/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "param_utils.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

void CheckAndCreateDir(const char *fileName)
{
    if (fileName == NULL || *fileName == '\0') {
        return;
    }
    char *path = strndup(fileName, strrchr(fileName, '/') - fileName);
    if (path != NULL && access(path, F_OK) != 0) {
        mkdir(path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    }
    free(path);
}

static void TrimString(char *string, uint32_t currLen)
{
    for (int i = currLen - 1; i >= 0; i--) {
        if (string[i] == ' ' || string[i] == '\0') {
            string[i] = '\0';
        } else {
            break;
        }
    }
}

int GetSubStringInfo(const char *buff, uint32_t buffLen, char delimiter, SubStringInfo *info, int subStrNumber)
{
    PARAM_CHECK(buff != NULL && info != NULL, return 0, "Invalid buff");
    size_t i = 0;
    // 去掉开始的空格
    for (; i < strlen(buff); i++) {
        if (!isspace(buff[i])) {
            break;
        }
    }
    // 过滤掉注释
    if (buff[i] == '#') {
        return -1;
    }
    // 分割字符串
    int spaceIsValid = 0;
    int curr = 0;
    int valueCurr = 0;
    for (; i < buffLen; i++) {
        if (buff[i] == '\n' || buff[i] == '\r' || buff[i] == '\0') {
            break;
        }
        if (buff[i] == delimiter && valueCurr != 0) {
            info[curr].value[valueCurr] = '\0';
            TrimString(info[curr].value, valueCurr);
            valueCurr = 0;
            curr++;
            spaceIsValid = 0;
        } else {
            if (!spaceIsValid && isspace(buff[i])) { // 过滤开始前的无效字符
                continue;
            }
            spaceIsValid = 1;
            if ((valueCurr + 1) >= (int)sizeof(info[curr].value)) {
                continue;
            }
            info[curr].value[valueCurr++] = buff[i];
        }
        if (curr >= subStrNumber) {
            break;
        }
    }
    if (valueCurr > 0) {
        info[curr].value[valueCurr] = '\0';
        TrimString(info[curr].value, valueCurr);
        valueCurr = 0;
        curr++;
    }
    return curr;
}