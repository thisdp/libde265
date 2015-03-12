/*
 * H.265 video codec.
 * Copyright (c) 2013-2014 struktur AG, Dirk Farin <farin@struktur.de>
 *
 * Authors: struktur AG, Dirk Farin <farin@struktur.de>
 *          Min Chen <chenm003@163.com>
 *
 * This file is part of libde265.
 *
 * libde265 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libde265 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libde265.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DE265_CONTEXTMODEL_H
#define DE265_CONTEXTMODEL_H

#include "libde265/cabac.h"
#include "libde265/de265.h"

#include <string.h>


typedef struct {
  uint8_t MPSbit : 1;
  uint8_t state  : 7;
} context_model;


enum context_model_index {
  // SAO
  CONTEXT_MODEL_SAO_MERGE_FLAG = 0,
  CONTEXT_MODEL_SAO_TYPE_IDX   = CONTEXT_MODEL_SAO_MERGE_FLAG +1,

  // CB-tree
  CONTEXT_MODEL_SPLIT_CU_FLAG  = CONTEXT_MODEL_SAO_TYPE_IDX + 1,
  CONTEXT_MODEL_CU_SKIP_FLAG   = CONTEXT_MODEL_SPLIT_CU_FLAG + 3,

  // intra-prediction
  CONTEXT_MODEL_PART_MODE      = CONTEXT_MODEL_CU_SKIP_FLAG + 3,
  CONTEXT_MODEL_PREV_INTRA_LUMA_PRED_FLAG = CONTEXT_MODEL_PART_MODE + 4,
  CONTEXT_MODEL_INTRA_CHROMA_PRED_MODE    = CONTEXT_MODEL_PREV_INTRA_LUMA_PRED_FLAG + 1,

  // transform-tree
  CONTEXT_MODEL_CBF_LUMA                  = CONTEXT_MODEL_INTRA_CHROMA_PRED_MODE + 1,
  CONTEXT_MODEL_CBF_CHROMA                = CONTEXT_MODEL_CBF_LUMA + 2,
  CONTEXT_MODEL_SPLIT_TRANSFORM_FLAG      = CONTEXT_MODEL_CBF_CHROMA + 4,

  // residual
  CONTEXT_MODEL_LAST_SIGNIFICANT_COEFFICIENT_X_PREFIX = CONTEXT_MODEL_SPLIT_TRANSFORM_FLAG + 3,
  CONTEXT_MODEL_LAST_SIGNIFICANT_COEFFICIENT_Y_PREFIX = CONTEXT_MODEL_LAST_SIGNIFICANT_COEFFICIENT_X_PREFIX + 18,
  CONTEXT_MODEL_CODED_SUB_BLOCK_FLAG          = CONTEXT_MODEL_LAST_SIGNIFICANT_COEFFICIENT_Y_PREFIX + 18,
  CONTEXT_MODEL_SIGNIFICANT_COEFF_FLAG        = CONTEXT_MODEL_CODED_SUB_BLOCK_FLAG + 4,
  CONTEXT_MODEL_COEFF_ABS_LEVEL_GREATER1_FLAG = CONTEXT_MODEL_SIGNIFICANT_COEFF_FLAG + 42,
  CONTEXT_MODEL_COEFF_ABS_LEVEL_GREATER2_FLAG = CONTEXT_MODEL_COEFF_ABS_LEVEL_GREATER1_FLAG + 24,

  CONTEXT_MODEL_CU_QP_DELTA_ABS        = CONTEXT_MODEL_COEFF_ABS_LEVEL_GREATER2_FLAG + 6,
  CONTEXT_MODEL_TRANSFORM_SKIP_FLAG    = CONTEXT_MODEL_CU_QP_DELTA_ABS + 2,

  // motion
  CONTEXT_MODEL_MERGE_FLAG             = CONTEXT_MODEL_TRANSFORM_SKIP_FLAG + 2,
  CONTEXT_MODEL_MERGE_IDX              = CONTEXT_MODEL_MERGE_FLAG + 1,
  CONTEXT_MODEL_PRED_MODE_FLAG         = CONTEXT_MODEL_MERGE_IDX + 1,
  CONTEXT_MODEL_ABS_MVD_GREATER01_FLAG = CONTEXT_MODEL_PRED_MODE_FLAG + 1,
  CONTEXT_MODEL_MVP_LX_FLAG            = CONTEXT_MODEL_ABS_MVD_GREATER01_FLAG + 2,
  CONTEXT_MODEL_RQT_ROOT_CBF           = CONTEXT_MODEL_MVP_LX_FLAG + 1,
  CONTEXT_MODEL_REF_IDX_LX             = CONTEXT_MODEL_RQT_ROOT_CBF + 1,
  CONTEXT_MODEL_INTER_PRED_IDC         = CONTEXT_MODEL_REF_IDX_LX + 2,
  CONTEXT_MODEL_CU_TRANSQUANT_BYPASS_FLAG = CONTEXT_MODEL_INTER_PRED_IDC + 5,
  CONTEXT_MODEL_TABLE_LENGTH           = CONTEXT_MODEL_CU_TRANSQUANT_BYPASS_FLAG + 1
};



typedef context_model context_model_table[CONTEXT_MODEL_TABLE_LENGTH];

inline void copy_context_model_table(context_model_table dst, context_model_table src)
{
  memcpy(dst,src,sizeof(context_model_table));
}



void initialize_CABAC_models(context_model context_model_table[CONTEXT_MODEL_TABLE_LENGTH],
                             int initType,
                             int QPY);


class context_model_table2
{
 public:
  context_model_table2();
  context_model_table2(const context_model_table2&);
  ~context_model_table2();

  void init(int initType, int QPY);
  void release();
  void decouple();
  context_model_table2 transfer();
  context_model_table2 copy() const { context_model_table2 t=*this; t.decouple(); return t; }

  context_model& operator[](int i) { return model[i]; }

  context_model_table2& operator=(const context_model_table2&);

  //void set_may_be_modified(bool flag=true) { mMayBeModified=flag; }
  //bool may_be_modified() const { return mMayBeModified; }

 private:
  void decouple_or_alloc_with_empty_data();

  context_model* model; // [CONTEXT_MODEL_TABLE_LENGTH]
  int* refcnt;

  bool mMayBeModified;
};


#endif
