/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the 
"Apache License"); you may not use this file except in compliance with the 
Apache License. You may obtain a copy of the Apache License at 
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Version: v2016.2.1  Build: 5995
  Copyright (c) 2006-2016 Audiokinetic Inc.
*******************************************************************************/

/// \file 
/// Audiokinetic's definitions and factory of overridable Memory Manager module.

#pragma once

#include "../../SoundEngine/Common/AkTypes.h"

/// \name Audiokinetic Memory Manager's implementation-specific definitions.
//@{
/// Memory Manager's initialization settings.
/// \sa AK::MemoryMgr
struct AkMemSettings
{
    AkUInt32 uMaxNumPools;              ///< Maximum number of memory pools.
};
//@}

namespace AK
{
    /// \name Audiokinetic implementation-specific modules factories.
    //@{
	namespace MemoryMgr
	{
	    /// Memory Manager initialization.
	    /// \sa AK::MemoryMgr
		AK_EXTERNAPIFUNC(AKRESULT, Init)(
			AkMemSettings * in_pSettings        ///< Memory manager initialization settings.
			);
	}
    //@}
}
