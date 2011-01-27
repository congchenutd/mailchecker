#include "Setting.h"

UserSetting::UserSetting(const QString& fileName) : MySetting<UserSetting>(fileName)
{
	if(QFile(this->fileName).size() == 0)   // no setting
		loadDefaults();
}

void UserSetting::loadDefaults()
{
	setValue("Interval", 15);
	setValue("Timeout", 30);
	setValue("Popup", true);
	setValue("Sound", QString());
	setValue("Application", QString());
	setValue("SoundFiles", QString());
	setValue("ApplicationFiles", QString());
	setValue("SSL", true);
}
