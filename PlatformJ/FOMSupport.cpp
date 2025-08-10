
#include <vector>
#include <string>


#include "FOMSupport.h"


int numParseFails = 0;
bool b_VMFParseError = false;




void PrintBitString(std::vector<bool>* bitstring, int startBit)
{
    printf("bitstring is length %d\r\n", (int)bitstring->size());
    std::string  s = "";
    for (int i = startBit; i < bitstring->size();i++) {
        if (bitstring->at(i)) s.append("1");
        else s.append("0");
        if (!((i + 1) % 8)) s.append(" ");
        if (!((i + 1) % 32)) s.append("\r\n");
    }
    printf("%s\r\n", s.c_str());
}



//First step in parsing a FxM message presented as a byte array
// must be even number of bytes
//for each pair, the LSB is the second bytes
std::vector<bool>* FxM_BuildBitStringFromByteArray(const unsigned char* m, int len) //len is number of bytes
{
    std::vector<bool>* bitstring = new std::vector<bool>;
    for (int i = 0; i < len; i += 2)
    {
        //the LSByte is pushed first
        unsigned char cb = m[i + 1];
        for (int x = 0; x < 8; x++)
        {
            if (cb & (0b00000001 << x)) bitstring->push_back(true); //the LSB is pushed first
            else bitstring->push_back(false);
        }

        //the MSByte is pushed second
        cb = m[i];
        for (int x = 0; x < 8; x++)
        {
            if (cb & (0b00000001 << x)) bitstring->push_back(true); //the LSB is pushed first
            else bitstring->push_back(false);
        }
    }
    return bitstring;
}


bool calcFxMBool(std::vector<bool>* bitstring, int FxMWord, int bitPosInWord, const char* msg)
{
    int bitIndex = bitPosInWord + FxMWord * 16;
    if ((bitIndex + 1) > bitstring->size())
    {
        printf("BitString Overrun! curBit is %d, NumBits is %d, but sizeof bitstring is %d\r\n", bitPosInWord, 1, (int)bitstring->size());
        puts("Remainder of parse is invalid");
        numParseFails++;
        b_VMFParseError = true;
        return 0;
    }
    bool retVal = bitstring->at(bitIndex);

    if (nullptr != msg) printf("@@ Bool Check: word: %d, bitPosInWord: %d  bitIndex %d %c   %s\r\n", FxMWord, bitPosInWord, bitIndex, retVal ? 'T' : 'F', msg);

    return retVal;
}


int FxM_calcValue(std::vector<bool>* bitstring, int FxMWord, int _startBit, int NumBits, const char* msg)
{
    int startBit = FxMWord * 16 + _startBit;
    int retVal = 0;
    char s[80];
    s[0] = 0;
    int x;
    for (x = 0; x < NumBits; x++)
    {
        bitstring->at(startBit) ? s[x] = '1' : s[x] = '0';
        retVal += (0b0001 << (x)) * (bitstring->at(startBit++) ? 1 : 0);
    }
    s[x] = 0;

    if (nullptr!= msg) printf("@@ Value Check - word: %d, startBit: %d NumBits: %d  Value is %d\r\n",FxMWord,_startBit, NumBits,retVal);
    return retVal;
}



void ParseFOM03(struct _FOM03& FOM03, std::vector<bool>* bitstring, int FxM_LengthInWords)
{
    printf("Rx FOM03 - Ongoing BIT & Status FOM of length %d (words)\r\n", FxM_LengthInWords);

    FOM03.ETRNR = calcFxMBool(bitstring, 0, 15);//, "ETR NOT RECEIVED"); //ETR NOT RECEIVED (ETR NR)

    FOM03.TG = calcFxMBool(bitstring, 0, 14); // TEST GO (TG) 
    FOM03.TF = calcFxMBool(bitstring, 0, 13); // TERMINAL FAIL (TF)
    FOM03.NKL = calcFxMBool(bitstring, 0, 12); // NO KEYS LOADED (NKL)
    FOM03.IPFF = calcFxMBool(bitstring, 0, 11);//IPF FAIL (IPFF) 
    FOM03.TDMA_RX_Fail = calcFxMBool(bitstring, 0, 10); // TDMA RECEIVE FAIL(TDMA RX FAIL)
    FOM03.TDMA_RX_DP = calcFxMBool(bitstring, 0, 9); //TDMA RECEIVE DEGRADED PERFORMANCE (TDMA RX DP)
    FOM03.TDMA_TX_FAIL = calcFxMBool(bitstring, 0, 8); //TDMA TRANSMIT FAIL (TDMA TX FAIL)
    FOM03.TDMA_TX_DP = calcFxMBool(bitstring, 0, 7); //TDMA TRANSMIT DEGRADED PERFORMANCE 
    FOM03.NET_DP = calcFxMBool(bitstring, 0, 7);  //NETDP - NETWORK/COMMUNICATION DEGRADED PERFORMANCE 
    FOM03.TOL = calcFxMBool(bitstring, 0, 5); //thermal overload

    FOM03.CurrentInitState = FxM_calcValue(bitstring, 1, 11, 3);//, "CurrentInitState");
    FOM03.NetEntryStatus = FxM_calcValue(bitstring, 1, 7, 3);
    FOM03.Init_Sets_Status = FxM_calcValue(bitstring, 1, 5, 2);
    FOM03.NR = calcFxMBool(bitstring, 1, 3); //NAVIGATION RESET CONFIRMED (NR)
    FOM03.BIT_Status = FxM_calcValue(bitstring, 1, 1, 2);

    FOM03.ControlPG_Status = FxM_calcValue(bitstring, 2, 13, 2);
    FOM03.LTTI = calcFxMBool(bitstring, 2, 10);//, "LTTI");
    FOM03.VIP1 = FxM_calcValue(bitstring, 2, 6, 2);
    FOM03.VCBSTAT = FxM_calcValue(bitstring, 2, 3, 3);
    FOM03.VCASTAT = FxM_calcValue(bitstring, 2, 0, 3);

}

void PrintFOM03(struct _FOM03 &FOM03)
{

    printf("ETRNR: %c\r\n", FOM03.ETRNR ? 'T' : 'F');
    printf("TG: %c\r\n", FOM03.TG ? 'T' : 'F');
    printf("TF: %c\r\n", FOM03.TF ? 'T' : 'F');
    printf("NKL: %c\r\n", FOM03.NKL ? 'T' : 'F');
    printf("IPFF: %c\r\n", FOM03.IPFF ? 'T' : 'F');
    printf("TOL: %c\r\n", FOM03.TOL ? 'T' : 'F');

    printf("CurrentInitState: %d which is %s\r\n", FOM03.CurrentInitState , FOM03.CurrentInitState_SS[FOM03.CurrentInitState]);
    printf("Init_Sets_Status: %d which is %s\r\n", FOM03.Init_Sets_Status, FOM03.InitSetsStatus_SS[FOM03.Init_Sets_Status]);

    printf("BIT_Status: %d which is %s\r\n", FOM03.BIT_Status, FOM03.BS[FOM03.BIT_Status]);
    printf("Init Set Status: %d which is %s\r\n", FOM03.Init_Sets_Status, FOM03.InitSetsStatus_SS[FOM03.Init_Sets_Status]);
    printf("Net Entry Status: %d which is %s\r\n", FOM03.NetEntryStatus, FOM03.NE_STATUS[FOM03.NetEntryStatus]);

    printf("NR - NAVIGATION RESET CONFIRMED: %c\r\n", FOM03.NR ? 'T' : 'F');
    printf("Control PG_Status: %d which is %s\r\n", FOM03.ControlPG_Status, FOM03.CtrlPG_STATUS[FOM03.ControlPG_Status]);
    printf("LTTI: %c\r\n", FOM03.LTTI ? 'T' : 'F');
    printf("VOICE INDICATORS PORT 1 (VIP1): %d which is %s\r\n", FOM03.VIP1, FOM03.VIP1_STR[FOM03.VIP1]);
    printf("Voice Channel A Status: %d which is %s\r\n", FOM03.VCASTAT, FOM03.VC_STATUS[FOM03.VCASTAT]);
    printf("Voice Channel B Status: %d which is %s\r\n", FOM03.VCBSTAT, FOM03.VC_STATUS[FOM03.VCBSTAT]);
}


void ParseFOM04(struct _FOM04 &FOM04, std::vector<bool>* bitstring, int FxM_LengthInWords)
{
    printf("Rx FOM04 - Initialization & Status Data Response FOM (KMR = 0) of length %d (words)\r\n", FxM_LengthInWords);

    FOM04.RequestNumber = FxM_calcValue(bitstring, 0, 9, 5);
    FOM04.RequestType = FxM_calcValue(bitstring, 0, 6, 3);
    FOM04.STAT = calcFxMBool(bitstring, 0, 5); 
    FOM04.DataWordCount = FxM_calcValue(bitstring, 0, 0, 5);

    FOM04.StartingDataWord = FxM_calcValue(bitstring, 1, 0, 12);

    if (0 == FOM04.KMR)
    {
        for (int i = 0;i < FOM04.DataWordCount;i++)
        {
            FOM04.IMS_DataWord[i] = FxM_calcValue(bitstring, 2 + i, 0, 16);
        }
    }

    else if ((1 == FOM04.KMR) || (2 == FOM04.KMR))
    {
        for (int i = 0;i < 12;i+=2)
        {
            FOM04.ShortTitle[i] =   FxM_calcValue(bitstring, 1 + i, 0, 8);
            FOM04.ShortTitle[i+1] = FxM_calcValue(bitstring, 1 + i, 8, 8);
        }
        FOM04.ShortTitle[24] = 0;

        for (int i = 0;i < 6;i += 2)
        {
            FOM04.Edition[i] =     FxM_calcValue(bitstring, 13 + i, 0, 8);
            FOM04.Edition[i + 1] = FxM_calcValue(bitstring, 13 + i, 8, 8);
        }
        FOM04.Edition[6] = 0;

        for (int i = 0;i < 16;i += 2)
        {
            FOM04.WII[i] =      FxM_calcValue(bitstring, 17 + i, 0, 8);
            FOM04.WII[i + 1] =  FxM_calcValue(bitstring, 17 + i, 8, 8);
        }
        FOM04.WII[16] = 0;

        for (int i = 0;i < 16;i += 2)
        {
            FOM04.KeySlotRefID[i] =     FxM_calcValue(bitstring, 25 + i, 0, 8);
            FOM04.KeySlotRefID[i + 1] = FxM_calcValue(bitstring, 25 + i, 8, 8);
        }
        FOM04.KeySlotRefID[4] = 0;

        FOM04.cryptoPeriod = FxM_calcValue(bitstring, 27, 0, 8);
        FOM04.KeyTagCount = FxM_calcValue(bitstring, 28, 0, 8);
        FOM04.KeyTagNumber = FxM_calcValue(bitstring, 28, 8, 8);



    }
    

}

void PrintFOM04(_FOM04 &FOM04)
{
    printf("Request Number: %d\r\n", FOM04.RequestNumber);
    printf("Request Type: %d which is %s\r\n", FOM04.RequestType, FOM04.RequestType_SS[FOM04.RequestType]);
    printf("FOM04_KMR0.STAT: %s\r\n", FOM04.STAT? FOM04.STAT_SS[1]: FOM04.STAT_SS[0]);
    printf("Data Word Count: %d\r\n", FOM04.DataWordCount);

    printf("Starting Data Word: %d \r\n", FOM04.StartingDataWord);

    if (0 == FOM04.KMR)
    {
        for (int i = 0;i < FOM04.DataWordCount;i++)
        {
            printf("SDW%04d %04X\r\n", FOM04.StartingDataWord + i, FOM04.IMS_DataWord[i]);
        }
    }

    else if ((1 == FOM04.KMR) || (2 == FOM04.KMR))
    {
        if (1 == FOM04.KMR)
        {
            printf("KMR == 1: ");
        }
        else if (2 == FOM04.KMR)
        {
            printf("KMR == 2: ");
        }

        //Common
        printf("ST: %s", FOM04.ShortTitle);
        printf("Ed: %s", FOM04.Edition);
        printf("WII: %s", FOM04.WII);
        printf("KSRID: %d", FOM04.KeySlotRefID);

        printf("Crypto Period: %d", FOM04.cryptoPeriod);
        printf("Key Tag Number: %d", FOM04.KeyTagNumber);
        printf("Key Tag Count: %d", FOM04.KeyTagCount);
    }


}


void ParseFOM05(struct _FOM05& FOM05, std::vector<bool>* bitstring, int FxM_LengthInWords)
{
    printf("Rx FOM05 - Init Data Change Acknowledgement FOM of length %d (words)\r\n", FxM_LengthInWords);

    FOM05.RequestNumber = FxM_calcValue(bitstring, 0, 9, 5);
    FOM05.STATUS = FxM_calcValue(bitstring, 0, 7, 2);
    FOM05.DataWordCount = FxM_calcValue(bitstring, 0, 0, 5);
    FOM05.StartingDataWord = FxM_calcValue(bitstring, 1, 0, 12);

}

void PrintFOM05(struct _FOM05& FOM05)
{
    printf("Request Number: %d\r\n", FOM05.RequestNumber);
    printf("Status: %s\r\n", FOM05.STATUS_SS[FOM05.STATUS]);
    printf("Data Word Count: %d\r\n", FOM05.DataWordCount);
    printf("Starting Data Word: %d \r\n", FOM05.StartingDataWord);
}



void ParseFOM06(struct _FOM06& FOM06, std::vector<bool>* bitstring, int FxM_LengthInWords)
{
    printf("Rx FOM06 - Init Data Change Notification FOM of length %d (words)\r\n", FxM_LengthInWords);

    FOM06.DataWordCount = FxM_calcValue(bitstring, 0, 0, 5);
    FOM06.StartingDataWord = FxM_calcValue(bitstring, 1, 0, 12);


}

void PrintFOM06(struct _FOM06& FOM06)
{
    printf("Data Word Count: %d\r\n", FOM06.DataWordCount);
    printf("Starting Data Word: %d \r\n", FOM06.StartingDataWord);
}

