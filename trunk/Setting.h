#ifndef Setting_h__
#define Setting_h__

#include "../MySetting/MySetting.h"

class UserSetting : public MySetting<UserSetting>
{
public:
	UserSetting(const QString& fileName);

private:
	void loadDefaults();
};

#endif // Setting_h__
