#include <bits/stdc++.h>

#include <http/http.hh>
#include <signal.h>
#include <unistd.h>
#include <cstring>

using namespace std;

string load_file(string path)
{
	ifstream ifs(path);

	if (!ifs) {
		throw runtime_error("No such file");
	}

	return string((istreambuf_iterator<char>(ifs)),
	              (istreambuf_iterator<char>()));
}

int from_hex(char c)
{
	if ('A' <= c && c <= 'Z') {
		return c - 'A' + 10;
	} else if ('a' <= c && c <= 'z') {
		return c - 'a' + 10;
	} else {
		return c - '0';
	}
}

map <string, string> parse_post_params(vector <char> const& sv)
{
	string s(sv.begin(), sv.end());
	string res;
	for (int i = 0; i < (int) s.length(); i++) {
		if (s[i] == '+') {
			res += " ";
		} else if (i < (int) s.length() - 2 && s[i] == '%') {
			// Decode hex number
			res += from_hex(s[i + 1]) * 16 + from_hex(s[i + 2]);
			i += 2;
		} else {
			res += s[i];
		}
	}

	map <string, string> result;
	string key;
	string value;
	string* cur = &key;

	for (char c : res) {
		if (c == '&') {
			result[key] = value;
			key = "";
			value = "";
			cur = &key;
		} else if (c == '=') {
			cur = &value;
		} else {
			*cur += c;
		}
	}
	if (!key.empty()) {
		result[key] = value;
	}
	
	return result;
}

struct chat_message
{
	string author;
	string body;

	string to_html() const
	{
		string html = load_file("html/message.html");
		html = regex_replace(html, regex("AUTHOR"), author);
		string html_body = regex_replace(body, regex("\n"), "<br>");
		return regex_replace(html, regex("BODY"), html_body);
	}
};

struct simple_chat_server
{
	simple_chat_server(string const& addr, uint16_t port)
		: server(addr, [=](auto arg) { on_request(move(arg)); }, port)
	{
	}

	string create_messages(string const& author) const
	{
		string result = load_file("html/chat.html");
		string messages_html;
		for (auto const& m : messages) {
			messages_html += m.to_html();
		}
		result = regex_replace(result, regex("AUTHOR"), author);
		return regex_replace(result, regex("MESSAGES"), messages_html);
	}

	void on_request(http::maybe<http::peer>&& mpeer)
	{
		if (mpeer) {
			auto peer = mpeer.get();
			auto request = peer.get_request();
			auto uri = request.get_uri();
			auto body = request.get_text();
			http::response::builder builder;

			if (uri == "/") {
				builder.set_text(load_file("html/index.html"));
			} else if (uri == "/chat.html") {
				auto params = parse_post_params(body);
				string author = params["author"];
				if (params.count("message")) {
					string message = params["message"];
					messages.push_back({author, message});
				}
				builder.set_text(create_messages(author));
			} else {
				builder.set_status(http::response::NOT_FOUND);
				builder.set_comment("Not found");
				builder.set_text("Requested resource not found. <a href=\"/\"> Main page </a>");
			}

			peer.send(builder.create());
		}
	}

private:
	vector<chat_message> messages;
	http::server server;
};

int main()
{
	try {
		simple_chat_server server("127.0.0.1", 33333);
		http::main_loop::start();
	} catch (exception const& e) {
		cerr << e.what() << endl;
	}
}
