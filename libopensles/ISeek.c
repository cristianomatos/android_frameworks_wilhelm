/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Seek implementation */

#include "sles_allinclusive.h"


static SLresult ISeek_SetPosition(SLSeekItf self, SLmillisecond pos, SLuint32 seekMode)
{
    SL_ENTER_INTERFACE

    switch (seekMode) {
    case SL_SEEKMODE_FAST:
    case SL_SEEKMODE_ACCURATE:
        {
        // maximum position is a special value that indicates a seek is not pending
        if (SL_TIME_UNKNOWN == pos)
            pos = SL_TIME_UNKNOWN - 1;
        ISeek *this = (ISeek *) self;
        interface_lock_exclusive(this);
        if (pos != this->mPos) {
            this->mPos = pos;
            interface_unlock_exclusive_attributes(this, ATTR_POSITION);
        } else
            interface_unlock_exclusive(this);
        result = SL_RESULT_SUCCESS;
        }
        break;
    default:
        result = SL_RESULT_PARAMETER_INVALID;
        break;
    }

    SL_LEAVE_INTERFACE
}


static SLresult ISeek_SetLoop(SLSeekItf self, SLboolean loopEnable,
    SLmillisecond startPos, SLmillisecond endPos)
{
    SL_ENTER_INTERFACE

    if (!(startPos < endPos)) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        ISeek *this = (ISeek *) self;
        interface_lock_exclusive(this);
        this->mLoopEnabled = SL_BOOLEAN_FALSE != loopEnable; // normalize
        this->mStartPos = startPos;
        this->mEndPos = endPos;
        interface_unlock_exclusive_attributes(this, ATTR_TRANSPORT);
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult ISeek_GetLoop(SLSeekItf self, SLboolean *pLoopEnabled,
    SLmillisecond *pStartPos, SLmillisecond *pEndPos)
{
    SL_ENTER_INTERFACE

    if (NULL == pLoopEnabled || NULL == pStartPos || NULL == pEndPos) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        ISeek *this = (ISeek *) self;
        interface_lock_shared(this);
        SLboolean loopEnabled = this->mLoopEnabled;
        SLmillisecond startPos = this->mStartPos;
        SLmillisecond endPos = this->mEndPos;
        interface_unlock_shared(this);
        *pLoopEnabled = loopEnabled;
        *pStartPos = startPos;
        *pEndPos = endPos;
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static const struct SLSeekItf_ ISeek_Itf = {
    ISeek_SetPosition,
    ISeek_SetLoop,
    ISeek_GetLoop
};

void ISeek_init(void *self)
{
    ISeek *this = (ISeek *) self;
    this->mItf = &ISeek_Itf;
    this->mPos = (SLmillisecond) SL_TIME_UNKNOWN;
    this->mStartPos = (SLmillisecond) 0;
    this->mEndPos = (SLmillisecond) SL_TIME_UNKNOWN;
    this->mLoopEnabled = SL_BOOLEAN_FALSE;
}
