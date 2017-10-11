#include "generatefile.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <string>
#include "ipv4.h"
#include "Base64.h"

using namespace std;
queue<string> q_user_dot_rule_local_proxy;
queue<string> q_user_dot_rule_ip_rules;

void getcnip()
{
	const string filename = R"(delegated-apnic-latest)";

	ifstream rawdata(filename);
	if (rawdata) {
		ofstream add, del, ssr;
		string temp_str;
		queue<ip_list> cn_ip_queue;
		ofstream test;

		cout << "�ҵ� " + filename << endl;

		add.open(R"(.\out\add.txt)", ios::trunc);
		del.open(R"(.\out\del.txt)", ios::trunc);
		ssr.open(R"(.\out\chn_ip.txt)", ios::trunc);
		test.open(R"(.\out\test.txt)", ios::trunc);

		cout << "���ڷ���·�ɱ�..." << endl;

		while (getline(rawdata, temp_str, '\n')) {
			if (is_cn_ipv4(temp_str)) {
				cn_ip_queue.push(get_ip(temp_str));
			}
		}

		cout << "��������·�ɱ�..." << endl;

		const auto number_of_ip = cn_ip_queue.size();
		for (; !cn_ip_queue.empty(); cn_ip_queue.pop()) {
			const auto ip = cn_ip_queue.front().first_ip.str();
			const auto mask = cn_ip_queue.front().mask;
			add << "add " + ip + " mask " + mask + " default METRIC default IF default" << endl;
			del << "delete " + ip + " mask " + mask + " default METRIC default IF default" << endl;
			temp_str = cn_ip_queue.front().first_ip.str() + " " + cn_ip_queue.front().last_ip.str();
			ssr << temp_str << endl;
			test << cn_ip_queue.front().first_ip.str() << " " << cn_ip_queue.front().last_ip.str() << " " << cn_ip_queue.front().first_ip.ip_to_long() << " " << cn_ip_queue.front().last_ip.ip_to_long() << " " << cn_ip_queue.front().mask << " " << cn_ip_queue.front().Hosts << " " << cn_ip_queue.front().CIDR << endl;

			q_user_dot_rule_ip_rules.push(temp_str);
		}
		add.close();
		del.close();
		ssr.close();
		test.close();
		cout << "·�ɱ����ɳɹ�������" << number_of_ip << "����" << endl;
	}
	else {
		cout << "δ�ҵ� " + filename << endl;
	}
	cout << endl;
}
void gfwlist2pac()
{
	const string gfwlist = R"(gfwlist.txt)";
	const string userrule = R"(user-rule.txt)";

	const ifstream gfwdata(gfwlist);

	if (gfwdata)
	{
		cout << "�ҵ� " + gfwlist << endl;
		auto list = Base64::Decode(textfile2str(gfwlist));//decode gfwlist
		const ifstream userdata(userrule);
		if (userdata)
		{
			cout << "�ҵ� " + userrule << endl;
			list += textfile2str(userrule);
		}
		else
		{
			cout << "δ�ҵ� " + userrule << endl;
		}

		cout << "���ڷ���GFWList..." << endl;
		string temp_str;
		istringstream ss_list(list);
		queue<string> domains;

		while (getline(ss_list, temp_str, '\n'))
		{
			if (temp_str[0] != '!' && temp_str != "" && temp_str[0] != '[')
			{
				replace_all_distinct(temp_str, R"(\)", R"(\\)");//��"\"�滻��"\\"
				domains.push(temp_str);
			}

		}
		cout << "����" << domains.size() << "����" << endl;

		//output
		cout << "��������pac.txt..." << endl;
		ofstream pac;
		pac.open(R"(.\out\gfwlist.pac)", ios::trunc);
		pac << pac_front_str;
		while (!domains.empty())
		{
			if (domains.size() != 1)
				pac << "\t\"" + domains.front() + "\"," << endl;
			else
				pac << "\t\"" + domains.front() + "\"" << endl;
			domains.pop();
		}
		pac << pac_back_str1;
		pac << pac_back_str2;
		pac.close();
		cout << "pac.txt���ɳɹ���" << endl;
	}
	else
	{
		cout << "δ�ҵ� " + gfwlist << endl;
	}
	cout << endl;
}
void get_cn_domains()
{
	const string rawdata_name = R"(accelerated-domains.china.conf)";
	const string whitelist = R"(whitelist.txt)";
	queue<string> domains;
	string temp_str;

	ifstream domains_data(rawdata_name);
	if (domains_data)
	{
		cout << "�ҵ� " + rawdata_name << endl;
		ifstream whitelist_data(whitelist);
		if (whitelist_data)
		{
			cout << "�ҵ� " + whitelist << endl;
			while (getline(whitelist_data, temp_str, '\n'))
			{
				if (temp_str != "")
				{
					domains.push(temp_str);
					replace_all_distinct(temp_str, R"(*.)", R"(.)");//��"*."�滻��"."
					q_user_dot_rule_local_proxy.push(temp_str);
				}
			}
			cout << "�û��Զ���������" << domains.size() << "����" << endl;
		}
		else
		{
			cout << "δ�ҵ� " + whitelist << endl;
		}

		cout << "���ڷ��� accelerated-domains.china.conf ..." << endl;

		while (getline(domains_data, temp_str, '\n'))
		{
			if (temp_str != "")
			{
				//ȡ������
				const auto pos1 = temp_str.find('/');
				const auto pos2 = temp_str.rfind('/');
				temp_str = temp_str.substr(pos1 + 1, pos2 - pos1 - 1);

				q_user_dot_rule_local_proxy.push("." + temp_str);
				domains.push("*." + temp_str);
			}
		}
		cout << "����" << domains.size() << "����" << endl;

		//output
		cout << "��������CNdomains.txt..." << endl;
		ofstream cn_domains;
		cn_domains.open(R"(.\out\CNdomains.txt)", ios::trunc);
		while (!domains.empty())
		{
			cn_domains << domains.front() << endl;
			domains.pop();
		}
		cn_domains.close();
		cout << "CNdomains.txt���ɳɹ���" << endl;
	}
	else
	{
		cout << "δ�ҵ� " + rawdata_name << endl;
	}
	cout << endl;
}

void generate_user_dot_rule()
{
	if (q_user_dot_rule_ip_rules.empty() && q_user_dot_rule_local_proxy.empty()) {
		cout << "δ�ҵ� accelerated-domains.china.conf �� delegated-apnic-latest.txt" << endl;
		return;
	}
	cout << "��������user.rule..." << endl;
	ofstream user_dot_rule;
	user_dot_rule.open(R"(.\out\user.rule)", ios::trunc);
	user_dot_rule << user_dot_rule_front;
	//output remote proxy rule
	user_dot_rule << user_dot_rule_remote_proxy;

	//output local proxy rule
	user_dot_rule << user_dot_rule_local_proxy;
	while (!q_user_dot_rule_local_proxy.empty())
	{
		user_dot_rule << q_user_dot_rule_local_proxy.front() << " localproxy" << endl;
		q_user_dot_rule_local_proxy.pop();
	}
	user_dot_rule << endl;

	//output direct
	user_dot_rule << user_dot_rule_direct;

	//output reject
	user_dot_rule << user_dot_rule_reject;

	//output host list
	user_dot_rule << user_dot_rule_host;
	user_dot_rule << R"(localhost 127.0.0.1)" << endl;

	//output special rule
	user_dot_rule << user_dot_rule_special;

	//output ip rules
	user_dot_rule << user_dot_rule_ip;
	while (!q_user_dot_rule_ip_rules.empty())
	{
		user_dot_rule << q_user_dot_rule_ip_rules.front() << " localproxy" << endl;
		q_user_dot_rule_ip_rules.pop();
	}
	user_dot_rule << endl;
	user_dot_rule << user_dot_rule_local;
	//output end
	user_dot_rule.close();
	cout << "user.rule���ɳɹ���" << endl;
}