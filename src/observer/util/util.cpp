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

RC convertValue(Value *value1, Value *value2)
{
  RC rc = RC::SUCCESS;
  const AttrType value_type1 = value1->type;
  const AttrType value_type2 = value2->type;
  switch (value_type1) {
    //转换日期
    case DATES: {
      if (value_type2 != CHARS) {
        rc = RC::SCHEMA_FIELD_TYPE_MISMATCH;
        return rc;
      }
      int32_t date = -1;
      rc = string_to_date((char *)(value2->data), date);
      if (rc != RC::SUCCESS) {
        // LOG_WARN("XXX");  // TODO log correct log
        return rc;
      }
      value_destroy(value2);
      value_init_date(value2, date);
      break;
    }
    // 转换字符串
    case CHARS: {
      // if (value_type2 == INTS || value_type2 == FLOATS) {
      //   std::stringstream ss;
      //   char *chars;
      //   if (value_type2 == INTS) {
      //     ss << abs(*(int *)(value1->data));
      //   } else if (value_type2 == FLOATS) {
      //     ss << fabs(*(float *)(value1->data));
      //   }
      //   value_destroy(value1);
      //   value_init_string(value1, ss.str().c_str());
      // } else if (value_type2 == DATES) {
      //   int32_t date = -1;
      //   rc = string_to_date((char *)(value1->data), date);
      //   if (rc != RC::SUCCESS) {
      //     // LOG_WARN("XXX");  // TODO log correct log
      //     return rc;
      //   }
      //   value_destroy(value1);
      //   value_init_date(value1, date);
      // }
      // break;
    }
    //转换整数
    case INTS: {
      if (value_type2 == CHARS) {
        int res;
        res = atoi(static_cast<char *>(value1->data));
        value_destroy(value2);
        value_init_integer(value2, res);
      } else if (value_type2 == FLOATS) {
        float res = (float)*(int *)(value1->data);
        value_destroy(value1);
        value_init_float(value1, res);
      } else {
        rc = RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
      break;
    }
    //转换浮点数
    case FLOATS: {
      if (value_type2 == CHARS || value_type2 == INTS) {
        float res;
        if (value_type2 == CHARS) {
          res = atof((char *)(value2->data));
        } else if (value_type2 == INTS) {
          res = *(float *)(value2->data);
        }
        value_destroy(value2);
        value_init_float(value2, res);
      }
      break;
    }

    default:
      return rc;
  }
  return rc;
}
