#pragma once

#include "api/binance/bnbmanager.h"
#include "inputhandlers/handlefiles.h"
#include "inputhandlers/handlesockets.h"

// 3rd party
#include "spdlog/async.h"

// libraries
#include "./lib/common.h"
#include "./lib/types/destructive_copy_constructible.h"
#include "./lib/types/dynamicobject.h"
#include "./lib/utils/filemanagement/filehelper.h"
#include "./lib/utils/filemanagement/filestructs.h"
#include "./lib/utils/filemanagement/memorymappedjson.h"
#include "./lib/utils/threadpool.h"
#include "./lib/utils/time.h"
#include "./lib/utils/timemanager.h"

// all non library includes below
#include "controller/trader.h"