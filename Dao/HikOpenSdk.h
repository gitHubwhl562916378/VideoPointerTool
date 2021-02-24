/*
 * @Author: your name
 * @Date: 2021-02-23 11:37:07
 * @LastEditTime: 2021-02-24 09:43:36
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \VideoPlayer\Dao\HikOpenSdk.h
 */
//��װ����open api��Ƶ������ؽӿ�

#pragma once

#include <functional>
#include "Poco/Net/HttpRequest.h"
#include "Poco/DigestEngine.h"
#include "openssl/sha.h"
#include "openssl/crypto.h"
#include "../Service/servicei.h"

class HikOpenSdk {
public:
	HikOpenSdk();

	void BindLogger(const std::function<void(const std::string&, const std::string&)>& logger);

	void set_app_key(const std::string& key) {
		app_key_ = key;
	}

	void set_app_secret(const std::string& secret) {
		app_secret_ = secret;
	}

	QString QueryCameraList(int page, int size, RestServiceI::CameraInfo* cameras);

	QString QueryURL(const QString& camera_id, QString* url);

private:
	bool RequestData(const std::string& uri, const std::string& body, std::string& resp);
	void GenerateDigestSignature(std::string& signature, std::map<std::string, std::string>& header, const std::string& path, const std::string& body);

private:
	std::string address_;
	std::string query_cam_path_;
	std::string query_url_path_;

	std::string app_key_;
	std::string app_secret_;

	std::function<void(const std::string&, const std::string&)> logger_;
};

using Poco::DigestEngine;

class KLSHA256Engine : public Poco::DigestEngine {
public:
	enum
	{
		BLOCK_SIZE = SHA256_CBLOCK,
		DIGEST_SIZE = SHA256_DIGEST_LENGTH
	};

	KLSHA256Engine();
	~KLSHA256Engine();

	std::size_t digestLength() const;
	void reset();
	const DigestEngine::Digest& digest();

protected:
	void updateImpl(const void* data, std::size_t length);

	SHA256_CTX ctx_;

	DigestEngine::Digest digest_;
};

