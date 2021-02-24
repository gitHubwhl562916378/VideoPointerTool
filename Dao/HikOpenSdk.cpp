//implement HikOpenSdk

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include "Poco/URI.h"
#include "Poco/Format.h"
#include "Poco/NumberParser.h"
#include "Poco/Base64Encoder.h"
#include "Poco/Base64Decoder.h"
#include "Poco/MD5Engine.h"
#include "Poco/HMACEngine.h"
#include "Poco/Base64Encoder.h"
#include "Poco/Base64Decoder.h"
#include "httplib.h"
#include "HikOpenSdk.h"

using Poco::MD5Engine;
using Poco::DigestEngine;
using Poco::HMACEngine;
using Poco::Base64Encoder;

HikOpenSdk::HikOpenSdk() {
	address_ = "220.163.129.62:1443";
	app_key_ = "21355400";
	app_secret_ = "GJTYYwZ35WhvZI4jsMHg";
	query_cam_path_ = "/artemis/api/resource/v1/cameras";
	query_url_path_ = "/artemis/api/video/v1/cameras/previewURLs";
}

void HikOpenSdk::BindLogger(const std::function<void(const std::string&, const std::string&)>& logger) {
	logger_ = logger;
}

QString HikOpenSdk::QueryCameraList(int page, int size, RestServiceI::CameraInfo* cameras) {
	std::string uri_str("https://");
	uri_str += address_;
	uri_str += query_cam_path_;

	QJsonObject rqst_json;
	rqst_json.insert("pageNo", page);
	rqst_json.insert("pageSize", size);
	rqst_json.insert("treeCode", "0");
	QJsonDocument jsDoc(rqst_json);
	QByteArray q_body_byte = jsDoc.toJson();

	std::string resp;
	if (!RequestData(uri_str, q_body_byte.toStdString(), resp))
		return "RequestData failed";

	QJsonParseError jsError;
	jsDoc = QJsonDocument::fromJson(QByteArray::fromStdString(resp), &jsError);
	if(jsError.error != QJsonParseError::NoError){
        return jsError.errorString();
    }

	QJsonObject jsObj = jsDoc.object();
	QString code = jsObj.value("code").toString();
	if(code != "0")
	{
		return jsObj.value("msg").toString();
	}

	QJsonObject js_data = jsObj.value("data").toObject();
	cameras->pageNo = js_data.value("pageNo").toInt();
	cameras->total = js_data.value("total").toInt();
	QJsonArray js_data_list = js_data.value("list").toArray();
    std::transform(js_data_list.begin(),js_data_list.end(),std::back_inserter(cameras->datas),[this](QJsonValue val){
		RestServiceI::CameraData data;
		
        QJsonObject js_camera_obj = val.toObject();
		data.cameraIndexCode = js_camera_obj.value("cameraIndexCode").toString();
		QueryURL(data.cameraIndexCode, data.rtsp);
		data.cameraName = js_camera_obj.value("name").toString();
		data.cameraTypeName = js_camera_obj.value("cameraTypeName").toString();
		data.recordLocationName = js_camera_obj.value("recordLocationName").toString();
		data.status = js_camera_obj.value("status").toInt();
		data.statusName = js_camera_obj.value("statusName").toString();
		data.treatyTypeName = js_camera_obj.value("treatyTypeName").toString();
        return data;
    });

	return QString();
}

QString HikOpenSdk::QueryURL(const QString& camera, QString& url) {
	QJsonObject q_js_body;
	q_js_body.insert("cameraIndexCode", camera);
	// q_js_body.insert("protocol", "rtmp");

	std::string uri_str("https://");
	uri_str += address_;
	uri_str += query_url_path_;

	QJsonDocument jsDoc(q_js_body);
	QByteArray q_body_byte = jsDoc.toJson();

	std::string resp;
	if (!RequestData(uri_str, q_body_byte.toStdString(), resp))
		return QString();

	QJsonParseError jsError;
	jsDoc = QJsonDocument::fromJson(QByteArray::fromStdString(resp), &jsError);
	if(jsError.error != QJsonParseError::NoError){
        return jsError.errorString();
    }

	QJsonObject js_resp_obj = jsDoc.object();
	QString code = js_resp_obj.value("code").toString();
	if(code != "0")
	{
		return js_resp_obj.value("msg").toString();
	}

	QJsonObject js_data = js_resp_obj.value("data").toObject();
	url = js_data.value("url").toString();

	return QString();
}

bool HikOpenSdk::RequestData(const std::string& uri, const std::string& body, std::string& resp) {

	Poco::URI q_uri(uri);

	std::string author_str = q_uri.getAuthority();
	std::string path = uri.substr(uri.find(author_str) + author_str.size());

	httplib::SSLClient http_client(q_uri.getHost(), q_uri.getPort());

	std::map<std::string, std::string> headers;
	headers.emplace("Accept", "*/*");
	headers.emplace("Content-Type", "application/json");

	std::string digest_signature;
	GenerateDigestSignature(digest_signature, headers, path, body);

	httplib::Headers http_headers;
	for (auto head : headers) {
		http_headers.emplace(head.first, head.second);
	}

	//std::cout << "GenerateDigestSignature: " << digest_signature << std::endl;

	http_headers.emplace("X-Ca-Key", app_key_);
	http_headers.emplace("X-Ca-Signature", digest_signature);
	http_headers.emplace("X-Ca-Signature-Headers", "x-ca-key");

	http_headers.emplace("Content-Length", std::to_string(body.size()));

	http_client.set_default_headers(http_headers);
	http_client.set_keep_alive(false);
	http_client.enable_server_certificate_verification(false);

	http_client.set_connection_timeout(2);
	http_client.set_read_timeout(5);

	auto res = http_client.Post(path.c_str(), body, "application/json");

	if (!res) {
		std::cout << "http client return error: " << res.error() << std::endl;
		return false;
	}

	if (res->status != 200) {
		std::cout << "http client return status " << res->status << " error: " << res.error() << std::endl;
		return false;
	}

	resp = res->body;
	return true;
}

void HikOpenSdk::GenerateDigestSignature(std::string& signature, std::map<std::string, std::string>& header, const std::string& path, const std::string& body) {

	std::string text("POST\n");
	auto iter = header.find("Accept");
	text += iter->second + "\n";

	// MD5Engine md5;
	// md5.update(body);
	// const DigestEngine::Digest& body_md5 = md5.digest();

	// std::ostringstream body_md5_base64;
	// Base64Encoder base64enc(body_md5_base64);
	// base64enc.write((char*)body_md5.data(), body_md5.size());
	// base64enc.close();
	// text += body_md5_base64.str() + "\n";

	iter = header.find("Content-Type");
	text += iter->second + "\n";
	text += "x-ca-key:" + app_key_ + "\n";

	text += path;

	HMACEngine<KLSHA256Engine> hmac(app_secret_);
	hmac.update(text);
	const DigestEngine::Digest& digest = hmac.digest();

	std::ostringstream signature_os;
	Base64Encoder signature_enc(signature_os);
	signature_enc.write((char*)digest.data(), digest.size());
	signature_enc.close();
	signature = signature_os.str();
}

KLSHA256Engine::KLSHA256Engine() {
	SHA256_Init(&ctx_);
}

KLSHA256Engine::~KLSHA256Engine() {
	OPENSSL_cleanse(&ctx_, sizeof(ctx_));
}

std::size_t KLSHA256Engine::digestLength() const {
	return SHA256_DIGEST_LENGTH;
}

void KLSHA256Engine::reset() {
	OPENSSL_cleanse(&ctx_, sizeof(ctx_));
	SHA256_Init(&ctx_);
}

const DigestEngine::Digest& KLSHA256Engine::digest() {
	std::uint8_t hash[SHA256_DIGEST_LENGTH];
	memset(hash, 0, SHA256_DIGEST_LENGTH);
	SHA256_Final(hash, &ctx_);
	digest_.clear();
	digest_.insert(digest_.begin(), hash, hash + SHA256_DIGEST_LENGTH);
	reset();
	return digest_;
}

void KLSHA256Engine::updateImpl(const void* data, std::size_t length) {
	SHA256_Update(&ctx_, data, length);
}