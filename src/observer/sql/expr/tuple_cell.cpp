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
// Created by WangYunlai on 2022/07/05.
//

#include "sql/expr/tuple_cell.h"
#include "storage/common/field.h"
#include "common/log/log.h"
#include "util/comparator.h"
#include "util/util.h"
#include "util/date.h"

void TupleCell::to_string(std::ostream &os) const
{
  switch (attr_type_) {
    case INTS: {
      os << *(int *)data_;
    } break;
    case FLOATS: {
      float v = *(float *)data_;
      os << double2string(v);
    } break;
    case CHARS: {
      for (int i = 0; i < length_; i++) {
        if (data_[i] == '\0') {
          break;
        }
        os << data_[i];
      }
    } break;
    case DATES: {
      int32_t i = * (int32_t *)data_;
      // os << date_to_string(i);
    } break;
    default: {
      LOG_WARN("unsupported attr type: %d", attr_type_);
    } break;
  }
}

int TupleCell::compare(const TupleCell &other) const
{
  int cmp;
  compare_type(this->data_,this->attr_type_,other.data_,other.attr_type_,&cmp);
  // if (this->attr_type_ == other.attr_type_) {
  //   switch (this->attr_type_) {
  //     case INTS:
  //       return compare_int(this->data_, other.data_);
  //     case FLOATS:
  //       return compare_float(this->data_, other.data_);
  //     case CHARS:
  //       return compare_string(this->data_, this->length_, other.data_, other.length_);
  //     default: {
  //       LOG_WARN("unsupported type: %d", this->attr_type_);
  //     }
  //   }
  // } else if (this->attr_type_ == INTS && other.attr_type_ == FLOATS) {
  //   float this_data = *(int *)data_;
  //   return compare_float(&this_data, other.data_);
  // } else if (this->attr_type_ == FLOATS && other.attr_type_ == INTS) {
  //   float other_data = *(int *)other.data_;
  //   return compare_float(data_, &other_data);
  // }
  // LOG_WARN("not supported");
  return cmp;  // TODO return rc?
}
