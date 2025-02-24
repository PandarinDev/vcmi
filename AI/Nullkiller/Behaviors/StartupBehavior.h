/*
* StartupBehavior.h, part of VCMI engine
*
* Authors: listed in file AUTHORS in main folder
*
* License: GNU General Public License v2.0 or later
* Full text of license available in license.txt file, in main folder
*
*/
#pragma once

#include "lib/VCMI_Lib.h"
#include "../Goals/CGoal.h"
#include "../AIUtility.h"

namespace NKAI
{
namespace Goals
{
	class StartupBehavior : public CGoal<StartupBehavior>
	{
	public:
		StartupBehavior()
			:CGoal(STARTUP)
		{
		}

		virtual TGoalVec decompose() const override;
		virtual std::string toString() const override;

		virtual bool operator==(const StartupBehavior & other) const override
		{
			return true;
		}
	};
}


}
