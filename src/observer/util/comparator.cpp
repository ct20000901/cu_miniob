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
// Created by wangyunlai on 2021/6/11.
//

#include <string.h>
#include <algorithm>
#include <stdint.h>
#include "comparator.h"
#include "util/date.h"

const double epsilon = 1E-6;

int compare_int(const void *arg1, const void *arg2)
{
  int v1 = *(int *)arg1;
  int v2 = *(int *)arg2;
  return v1 - v2;
}

int compare_float(const void *arg1, const void *arg2)
{
  float v1 = *(float *)arg1;
  float v2 = *(float *)arg2;
  float cmp = v1 - v2;
  if (cmp > epsilon) {
    return 1;
  }
  if (cmp < -epsilon) {
    return -1;
  }
  return 0;
}

int compare_string(const void *arg1, int arg1_max_length, const void *arg2, int arg2_max_length)
{
  const char *s1 = (const char *)arg1;
  const char *s2 = (const char *)arg2;
  int maxlen = std::min(arg1_max_length, arg2_max_length);
  int result = strncmp(s1, s2, maxlen);
  if (0 != result) {
    return result;
  }

  if (arg1_max_length > maxlen) {
    return s1[maxlen] - 0;
  }

  if (arg2_max_length > maxlen) {
    return 0 - s2[maxlen];
  }
  return 0;
}

RC compare_type(const void *arg1, AttrType attr_type1, const void *arg2, AttrType attr_type2, int *cmp)
{
  RC rc = RC::SUCCESS;
  switch (attr_type1) {
    case CHARS: {
      if (attr_type2 == INTS) {
        int i1 = atoi((char *)arg1);
        *cmp = compare_int(&i1, arg2);
      } else if (attr_type2 == FLOATS) {
        float i1 = atof((char *)arg1);
        *cmp = compare_float(&i1, arg2);
      } else if (attr_type2 == CHARS) {
        *cmp = strcmp((char *)arg1, (char *)arg2);
      } else {
        rc = RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
    } break;
    case INTS: {
      if (attr_type2 == FLOATS) {
        float i = *(int *)arg1;
        *cmp = compare_float(&i, arg2);
      } else if (attr_type2 == CHARS) {
        int i1 = atoi((char *)arg2);
        *cmp = compare_int(arg1, &i1);
      } else if (attr_type2 == INTS) {
        *cmp = compare_int(arg1, arg2);
      } else {
        rc = RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
    } break;
    case FLOATS: {
      if (attr_type2 == INTS) {
        float i = *(int *)arg2;
        *cmp = compare_float(arg1, &i);
      } else if (attr_type2 == CHARS) {
        float i = atof((char *)arg2);
        *cmp = compare_float(arg1, &i);
      } else if (attr_type2 == FLOATS) {
        *cmp = compare_float(arg1, arg2);
      } else {
        rc = RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
    } break;
    case DATES: {
      int32_t r_value;
      int i = 0;
      if (attr_type2 == CHARS) {
        rc = string_to_date((char *)arg2, r_value);
        if (rc != RC::SUCCESS) {
          return rc;
        }
      } else if (attr_type2 == DATES) {
        int i = (*(int32_t *)arg1) - r_value;
      } else {
        rc = RC::SCHEMA_FIELD_TYPE_MISMATCH;
        break;
      }
      if (i > 0) {
        *cmp = 1;
      }
      if (i == 0) {
        *cmp = 0;
      }
      if (i < 0) {
        *cmp = -1;
      }
    }

    default:
      rc = RC::SCHEMA_FIELD_TYPE_MISMATCH;
      break;
  }
  return rc;
}
