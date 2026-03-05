
#include <Notifications/Slacker.hpp>

using namespace KTN::notify;


Slacker::Slacker()
{
}

Slacker::~Slacker()
{
	_ACTIVE.store(false, std::memory_order_relaxed);
}

void Slacker::InitAndRun()
{
	Settings settings("Settings.txt");

	_ENABLED.store(true, std::memory_order_relaxed);

	_BOT.baseurl = settings.getString("Slack.BaseUrl");
	_BOT.channel = settings.getString("Slack.Channel");
	_BOT.color = settings.getString("Slack.Color");
	_BOT.token = settings.getString("Slack.Token");
	_BOT.username = settings.getString("Slack.Username");

	LogMe("SLACK BOT: " + _BOT.Print());

	//Named Thread Impl
	CreateAndRun("slacker",1);
}

void Slacker::LogMe(std::string szMsg, LogLevel::Enum level)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void Slacker::Enable(bool sendmessages)
{
	_ENABLED.store(sendmessages, std::memory_order_relaxed);

	if (_ENABLED.load(std::memory_order_relaxed) == true)
		LogMe("SLACKER ENABLED!!!", LogLevel::INFO);
	else
		LogMe("SLACKER DISABLED!!!", LogLevel::ERROR);

}

void Slacker::Send(string preview, string topic, string text, LogLevel::Enum, bool force)
{

	if (_ENABLED.load(std::memory_order_relaxed) == false && !force)
		return;

	char buffer[50];
	time_t tt = time(0);
	tm *tm = localtime(&tt);
	strftime(buffer, sizeof(buffer), "%m-%d-%Y %H:%M:%S", tm);
	string pre = buffer;

	text = pre + "| " + text;

	slackmsg msg = {};
	msg.topic = topic;
	msg.text = text;
	//msg.color = color;
	msg.preview = preview;

	_qSlack.push(msg);

}

void Slacker::Send(slackmsg& msg)
{

	if (_ENABLED.load(std::memory_order_relaxed) == false)
		return;

	char buffer[50];
	time_t tt = time(0);
	tm *tm = localtime(&tt);
	strftime(buffer, sizeof(buffer), "%m-%d-%Y %H:%M:%S", tm);
	string pre = buffer;

	string text = pre + "| " + msg.text;
	msg.text = text;

	_qSlack.push(msg);

}

void Slacker::Run()
{
	//int BATCH_SIZE = 50;

	_ACTIVE.store(true, std::memory_order_relaxed);
	while(_ACTIVE.load(std::memory_order_relaxed) == true)
	{
		while(!_qSlack.empty())
		{
			slackmsg msg = {};
			while(_qSlack.try_pop(msg))
			{
				BuildAndSend(msg);
				usleep(10000);
			}
		}

		usleep(10000);

//		if (_ENABLED.load(std::memory_order_relaxed) == true)
//		{
//			time_t rawtime;
//			struct tm * ptm;
//			time ( &rawtime );
//			ptm = gmtime ( &rawtime );
//			int hour = ptm->tm_hour;
//			int min = ptm->tm_min;
//			int sec = ptm->tm_sec;
//
//			if (hour == 0 && min == 0 && sec == 0 )
//			{
//				LogMe("ENABLING SLACK FROM TIMER!!!!!", LogLevel::INFO);
//				Enable(true);
//			}
//		}
	}
}


void Slacker::BuildAndSend(slackmsg msg)
{
	std::string baseurl;
	std::string path;

	string json = FormatJson(msg);
	int bytelen = json.length();

	string action = "POST";

	string url = _BOT.baseurl + _BOT.token;

	//LogMe("Building Slack to " + url);

	vector <string> extra_http_header;
	string header_chunk("'Content-Type':application/json");
	extra_http_header.push_back(header_chunk);

	ostringstream osby;
	osby << "'Content-Length':" << bytelen;
	extra_http_header.push_back(osby.str());

	string post_data = json;

	//LogMe("SENDING JSON TO HOOK");
	//cout << json << endl;

	string str_result;
	curl_api_with_header(url, str_result ,extra_http_header, post_data, action ) ;
}

string Slacker::FormatJson(slackmsg msg)
{
	StringBuffer s;
	Writer<StringBuffer> writer(s);

	//new webhook
	writer.StartObject();

		writer.Key("text");
		writer.String(msg.preview.c_str());

		writer.Key("username");
		writer.String(_BOT.username.c_str());

		writer.Key("channel");
		writer.String(_BOT.channel.c_str());

		writer.Key("attachments");
		writer.StartArray();

			writer.StartObject();

					writer.Key("color");
					string color = FormatColor(msg.color);
					writer.String(color.c_str());

					writer.Key("fields");
					writer.StartArray();

						writer.StartObject();

							writer.Key("fallback");
							writer.String("New message from Occam");

							writer.Key("pretext");
							writer.String("New message from Occam");

							writer.Key("title");
							writer.String(msg.topic.c_str());

							writer.Key("value");
							writer.String(msg.text.c_str());

							writer.Key("short");
							writer.Bool(false);

						writer.EndObject();

					writer.EndArray(); //fields

			writer.EndObject();

		writer.EndArray(); //attachments

	writer.EndObject(); //top level object

	std::string res = s.GetString();
	return res;
}

string Slacker::FormatColor(int color)
{
	//white (ish)
	string res = "#eeeee4";

	switch(color)
	{
	case LogLevel::INFO:
		res = "#1e81b0";
		break;
	case LogLevel::WARN:
		res = "#FFFF00";
		break;
	case LogLevel::ERROR:
		res = "#FF0000";
		break;
	case LogLevel::DEBUG:
		res = "#FF00FF";
		break;
	}
	return res;
}


//--------------------
// Do the curl
void
Slacker::curl_api_with_header( string &url, string &str_result, vector <string> &extra_http_header , string &post_data , string &action )
{
	CURL *curl;
	CURLcode res;
	std::string readBuffer;
	curl = curl_easy_init();

	if( curl ) {

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_cb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str_result );
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_ENCODING, "gzip");

//		if ( extra_http_header.size() > 0 ) {
//
//			struct curl_slist *chunk = NULL;
//			for ( int i = 0 ; i < extra_http_header.size() ;i++ ) {
//				chunk = curl_slist_append(chunk, extra_http_header[i].c_str() );
//			}
//			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
//		}

		if ( post_data.size() > 0 || action == "POST" || action == "PUT" || action == "DELETE" ) {

			if ( action == "PUT" || action == "DELETE" ) {
				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, action.c_str() );
			}
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str() );
 		}

		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		/* Check for errors */
		if ( res != CURLE_OK ) {
			ostringstream oss;
			oss << "<SLACKER::curl_api> curl_easy_perform() failed: " << curl_easy_strerror(res);
			KT01_LOG_ERROR("SLACKER", oss.str());
		}

	}
}

//-----------------
// Curl's callback
size_t
Slacker::curl_cb( void *content, size_t size, size_t nmemb, std::string *buffer )
{
	buffer->append((char*)content, size*nmemb);
	return size*nmemb;
}

