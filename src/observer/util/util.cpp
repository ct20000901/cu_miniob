/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by wangyunlai on 2022/9/28
//

#include <string.h>
#include <string>
#include <sstream>
#include <math.h>
#include <algorithm>
#include "sql/parser/parse_defs.h"
#include "util/util.h"
#include "util/date.h"

std::string double2string(double v)
{
  char buf[256];
  snprintf(buf, sizeof(buf), "%.2f", v);
  size_t len = strlen(buf);
  while (buf[len - 1] == '0') {
    len--;
  }
  if (buf[len - 1] == '.') {
    len--;
  }

  return std::string(buf, len);
}

RC convertValue2Field(Value *value, AttrType field_type)
{
  RC rc = RC::SUCCESS;
  const AttrType value_type = value->type;
  switch (field_type) {
    //转换日期
    case DATES: {
      if (value_type != CHARS) {
        rc = RC::SCHEMA_FIELD_TYPE_MISMATCH;
        return rc;
      }
      int32_t date = -1;
      rc = string_to_date((char *)(value->data), date);
      if (rc != RC::SUCCESS) {
        // LOG_WARN("XXX");  // TODO log correct log
        return rc;
      }
      value_destroy(value);
      value_init_date(value, date);
      break;
    }
    // 转换字符串
    case CHARS: {
      std::stringstream ss;
      char *chars;
      if (value_type == INTS) {
        ss << abs(*(int *)(value->data));
      } else if (value_type == FLOATS) {
        ss << fabs(*(float *)(value->data));
      }
      value_destroy(value);
      value_init_string(value, ss.str().c_str());
      break;
    }
    //转换整数
    case INTS: {
      if (value_type == CHARS || value_type == FLOATS) {
        int res;
        if (value_type == CHARS) {
          res = atoi(static_cast<char *>(value->data));
        } else if (value_type == FLOATS) {
          res = (int)(((*(float *)(value->data)) * 10 + 5) / 10);
        }
        value_destroy(value);
        value_init_integer(value, res);
      }
      break;
    }
    //转换浮点数
    case FLOATS: {
      if (value_type == CHARS || value_type == INTS) {
        float res;
        if (value_type == CHARS) {
          res = atof((char *)(value->data));
        } else if (value_type == INTS) {
          res = *(float *)(value->data);
        }
        value_destroy(value);
        value_init_float(value, res);
      }
      break;
    }

    default:
      return rc;
  }
  return rc;
}