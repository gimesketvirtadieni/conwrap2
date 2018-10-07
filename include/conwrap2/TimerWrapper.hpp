/*
 * Copyright 2018, Andrej Kislovskij
 *
 * This is PUBLIC DOMAIN software so use at your own risk as it comes
 * with no warranties. This code is yours to share, use and modify without
 * any restrictions or obligations.
 *
 * For more information see conwrap/LICENSE or refer refer to http://unlicense.org
 *
 * Author: gimesketvirtadieni at gmail dot com (Andrej Kislovskij)
 */

#pragma once

#include <memory>

#include <conwrap2/Timer.hpp>


namespace conwrap2
{
	// TODO: use generic value container
	class TimerWrapper
	{
		public:
			TimerWrapper(Timer&& t)
			: timerPtr{std::make_unique<Timer>(std::move(t))} {}

			inline auto& getTimer()
			{
				return *timerPtr;
			}

		private:
			std::unique_ptr<Timer> timerPtr;
	};
}
