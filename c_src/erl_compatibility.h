/*
   Copyright (c) 2012 Basho Technologies, Inc.

   This file is provided to you under the Apache License,
   Version 2.0 (the "License"); you may not use this file
   except in compliance with the License.  You may obtain
   a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing,
   software distributed under the License is distributed on an
   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
   KIND, either express or implied.  See the License for the
   specific language governing permissions and limitations
   under the License.

*/

#ifndef __ERL_COMPATIBILITY_H__
#define __ERL_COMPATIBILITY_H__

/* For compatibility with OTP releases prior to R15.
   R15 and greater uses ErlDrvSizeT as an unsigned size
   like size_t for driver_* calls
*/
#if ERL_DRV_EXTENDED_MAJOR_VERSION < 2
#define ErlDrvSizeT size_t
#define ErlDrvSSizeT ssize_t
#endif


#endif /* __ERL_COMPATIBILITY_H__ */