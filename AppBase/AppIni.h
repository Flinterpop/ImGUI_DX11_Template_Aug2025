#pragma once

#include <string>

void bg_SetIniDBFileName(const char* name);
bool bg_CreateAppIniIfDoesntExist();


bool SetIniInt(char* name, int val);
bool UpdateIniInt(char* name, int val);
int GetIniInt(char* name, int _default);

int GetIniBool(char* name, bool _default);
bool SetIniBool(char* name, bool _val);
bool UpdateIniBool(char* name, bool _val);

double GetIniReal(char* name, double _default);
bool SetIniReal(char* name, double val);
bool UpdateIniReal(char* name, double val);


std::string GetIniString(char* name, std::string _default);
bool UpdateIniString(char* name, char* val);


