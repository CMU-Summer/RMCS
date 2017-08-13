#include "ConfigManager.h"

ConfigManager::ConfigManager() {}

ConfigManager::ConfigManager(string filePath):groupList(),redisList(),dbList() {
	ptree pt;
	read_xml(filePath, pt);

	//loop for every node under <config>
	BOOST_FOREACH(ptree::value_type &v1, pt.get_child("config")){
		if (v1.first == "groups") {
			BOOST_FOREACH(ptree::value_type &v2, v1.second) {
				if (v2.first == "group") {  //for each group
					GroupStruct* group = new GroupStruct();
					cout << "Group is created" << endl;
					BOOST_FOREACH(ptree::value_type &v3, v2.second) {
						if (v3.first == "<xmlattr>") {  //it's an attribute
							(*group).setName(v3.second.get<string>("name"));
							cout << "    Group name is " << (*group).getName() << endl;
				
						}
						else if (v3.first == "family") {
							FamilyStruct* family = new FamilyStruct();
							cout << "Family is created" << endl;

							BOOST_FOREACH(ptree::value_type &v4, v3.second) {
								if (v4.first == "<xmlattr>") {  //it's an attribut
									(*family).setName(v4.second.get<string>("name"));
									cout << "        Family name is " << (*family).getName() << endl;
								}
								else
								{
									BOOST_FOREACH(ptree::value_type &v5, v4.second) {
										(*family).addName(v5.second.data());
										cout << "      family add name:" << v5.second.data() << endl;
									}
								}
							}
							// Add new family to tmp group
							
							group->addFamily((*family));
						}
					}
					// Add whole group to groupList
					groupList.push_back((*group));
				}
				else {
					cout << "Error! Check your config.xml format!!" << endl;
				}
				cout << endl;
				
			}
		}else if(v1.first == "redisList"){
			BOOST_FOREACH(ptree::value_type &v2, v1.second) {
				if(v2.first == "redis"){
					BOOST_FOREACH(ptree::value_type &v3, v2.second) {
						if(v3.first == "<xmlattr>"){
							RedisCofig redCog;
							redCog.ip=v3.second.get<string>("ip");
							redCog.port=v3.second.get<int>("port");
							this->redisList.push_back(redCog);
						}
					}
				}
		

			}




		}else if(v1.first == "databases"){
			BOOST_FOREACH(ptree::value_type &v2, v1.second) {
				if(v2.first == "database"){
					BOOST_FOREACH(ptree::value_type &v3, v2.second) {
						DBconfig db;
						if(v3.first == "<xmlattr>"){
							db.ip=v3.second.get<string>("ip");
							db.port=v3.second.get<int>("port");
							db.user=v3.second.get<string>("user");
							db.pwd=v3.second.get<string>("pwd");
							db.dbSchme=v3.second.get<string>("dbSchme");
	
						}else if(v3.first == "parms"){
							BOOST_FOREACH(ptree::value_type &v4, v3.second) {
								if(v4.first == "parm"){
									BOOST_FOREACH(ptree::value_type &v5, v4.second) {
										if(v5.first == "<xmlattr>"){
											db.parmMap[v5.second.get<string>("name")]=v5.second.get<string>("value");
										}

									}
								}
							}

						}
						this->dbList.push_back(db);
					}
				}
			}
		}
	}
}
ConfigManager::~ConfigManager() {


}

vector<GroupStruct> ConfigManager::getGroupList() {
	return groupList;
}

vector<RedisCofig> ConfigManager::getRedisList(){return this->redisList;}
vector<DBconfig> ConfigManager::getDbConfig(){return this->dbList;}
/*
// Test Demo
int main(void) {
	ConfigManager("resource/config.xml");
	system("pause");
}
*/

