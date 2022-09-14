#include "Result.h"
#include "GameUtility.h"
#include <GatesEngine/Header/GUI/GUIManager.h>
#include <cpprest/filestream.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <fstream>

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

const std::wstring BASE_URL = L"https://tendaysjamapi.herokuapp.com";

float Result::timer = 0;
bool Result::isStartTimer = false;

std::array<float, 8> Result::ranking;

GE::IGraphicsDeviceDx12* Result::graphicsDevice = nullptr;

const GE::Math::Vector3 SCALE_CLEAR_TIME = { 512, 96, 0 };
const GE::Math::Vector3 SCALE_NUMBER = { 32, 64, 0 };
const GE::Math::Vector3 SCALE_WORLD_RANKING = { 482, 64, 0 };
const GE::Math::Vector3 SCALE_RANKING_NUM = { 16, 32, 0 };

const GE::Math::Vector3 POS_BASE_MONITOR_LEFT = { 12, 348, 0 };
const GE::Math::Vector3 POS_BASE_MONITOR_RIGHT = { 1428, 144, 0 };

const GE::Math::Vector3 POS_CLEAR_TIME = POS_BASE_MONITOR_LEFT + GE::Math::Vector3(486, 96 + 10, 0) / 2;
const GE::Math::Vector3 POS_NUMBER_LEFT = POS_BASE_MONITOR_LEFT + GE::Math::Vector3(486 / 2 - 32 * 7 / 2, 150, 0) + SCALE_NUMBER / 2;
const GE::Math::Vector3 POS_WORLD_RANKING = POS_BASE_MONITOR_RIGHT + GE::Math::Vector3(486, 64 + 10, 0) / 2;
const GE::Math::Vector3 POS_RANKING_NUM_1ST = POS_BASE_MONITOR_RIGHT + GE::Math::Vector3(150, 80, 0) + SCALE_RANKING_NUM / 2;


void Result::Initialize()
{
	timer = 0;
	isStartTimer = false;
}

void Result::UpdateTimer(float deltaTime)
{
	if (GameUtility::GetGameState() == GameState::RESULT) {
		isStartTimer = true;
	}
	else {
		isStartTimer = false;
	}

	if (isStartTimer) { timer += deltaTime; }
}

void Result::Draw()
{
	if (timer >= 0.5f) {
		Draw(POS_CLEAR_TIME, SCALE_CLEAR_TIME * 0.8f, "ClearTime");
	}

	if (timer >= 1.0f) {
		std::string num = std::to_string(GameUtility::GetClearTime());
		DrawNum(num.substr(0, num.find(".") + 5), POS_NUMBER_LEFT, SCALE_NUMBER, 0);
	}

	if (timer >= 1.5f) {
		Draw(POS_WORLD_RANKING, SCALE_WORLD_RANKING * 0.8f, "WorldRanking");

		for (int i = 0; i < ranking.size(); i++) {
			std::string num = std::to_string(ranking[i]);
			std::string myTime = std::to_string(GameUtility::GetClearTime());

			//自分の記録があれば点滅させる
			if (num != myTime || (int)(timer * 2) % 2 == 0) {
				DrawNum(std::to_string(i + 1) + ": " + num.substr(0, num.find(".") + 5), POS_RANKING_NUM_1ST + GE::Math::Vector3(0, 32 * i, 0), SCALE_NUMBER / 2, 0);
			}
		}
	}
}

void Result::SendScore(float time)
{
	//ランキングDBにデータ登録
	try {
		bool result = pplx::create_task([=]
			{
				//クライアントの設定
				http_client client(BASE_URL + L"/score");

				//送信データの作成
				json::value postData;
				postData[L"score"] = json::value::number(time);

				//トークン設定用
				http_request request;
				request.set_method(methods::POST);
				request.set_body(postData.serialize(), L"application/json");

				//リクエスト送信
				return client.request(request);
			})
			.then([](http_response response) {
				//ステータスコード判定
				if (response.status_code() == status_codes::OK) {
					//jsonを返す
					return response.extract_json();
				}
				})
				.then([](json::value json) {
					return true;
					}).wait();
	}
	catch (...) {
		return;
	}
}

void Result::GetRanking()
{
	//ランキングデータベースからデータ取得
	//降順に5つデータを返すようになっている
	auto json = pplx::create_task([=]
		{
			//クライアントの設定
			http_client client(BASE_URL + L"/score");

			//リクエスト設定
			http_request request;
			request.set_method(methods::GET);

			//リクエスト送信
			return client.request(request);
		})
		.then([](http_response response) {
			//ステータスコード判定
			if (response.status_code() == status_codes::OK) {
				//jsonを返す
				return response.extract_json();
			}
			}).get();

			auto& array = json.as_array();

			//arrayに要素格納し返す
			std::array<float, 8> result = {};
			for (int i = 0; i < result.size(); i++) {
				if (i >= array.size()) {
					result[i] = 0;
					continue;
				}

				result[i] = array[i].at(U("score")).as_double();
			}

			ranking = result;
}

void Result::Draw(const GE::Math::Vector3& pos, const GE::Math::Vector3& scale, const std::string& name)
{
	GE::ICBufferAllocater* cbufferAllocater = graphicsDevice->GetCBufferAllocater();
	GE::RenderQueue* renderQueue = graphicsDevice->GetRenderQueue();

	graphicsDevice->SetShader("DefaultSpriteWithTextureShader");

	GE::Math::Matrix4x4 modelMatrix = GE::Math::Matrix4x4::Scale(scale);

	modelMatrix *= GE::Math::Matrix4x4::Translate(pos);
	GE::Material material;
	material.color = GE::Color::White();

	GE::CameraInfo cameraInfo;
	cameraInfo.viewMatrix = GE::Math::Matrix4x4::GetViewMatrixLookTo({ 0,1,0 }, { 0,0,1 }, { 0,1,0 });
	cameraInfo.projMatrix = GE::Math::Matrix4x4::GetOrthographMatrix(GE::Window::GetWindowSize());

	renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
	renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &cameraInfo, sizeof(GE::CameraInfo)) });
	renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2,&material,sizeof(GE::Material)) });
	renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get(name)->GetSRVNumber() });

	graphicsDevice->DrawMesh("2DPlane");
}

void Result::DrawNum(const std::string& num, const GE::Math::Vector3& pos, const GE::Math::Vector3& scale, float padding)
{
	for (int i = 0; i < num.size(); i++) {
		if (num[i] == '.') {
			Draw(pos + GE::Math::Vector3(i * scale.x + padding, 0, 0), scale, "Dot");
		}
		else if (num[i] == ':') {
			Draw(pos + GE::Math::Vector3(i * scale.x + padding, 0, 0), scale, "Colon");
		}
		else if (num[i] == ' ') {
			continue;
		}
		else {
			Draw(pos + GE::Math::Vector3(i * scale.x + padding, 0, 0), scale, "Number_" + num.substr(i, 1));
		}
	}
}
