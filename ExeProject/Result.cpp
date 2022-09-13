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

const std::wstring BASE_URL = L"https://tendaysjamapi.herokuapp.com/";

float Result::timer = 0;
bool Result::isStartTimer = false;

std::array<float, 5> Result::ranking;

GE::IGraphicsDeviceDx12* Result::graphicsDevice = nullptr;

const GE::Math::Vector3 SCALE_CLEAR_TIME = { 512, 96, 0 };
const GE::Math::Vector3 SCALE_NUMBER = { 32, 64, 0 };

const GE::Math::Vector3 POS_BASE_MONITOR_LEFT = { 12, 348, 0 };
const GE::Math::Vector3 POS_BASE_MONITOR_RIGHT = { 1428, 144, 0 };

const GE::Math::Vector3 POS_CLEAR_TIME = POS_BASE_MONITOR_LEFT + GE::Math::Vector3(16, 0, 0) + SCALE_CLEAR_TIME / 2;
const GE::Math::Vector3 POS_NUMBER_LEFT = POS_BASE_MONITOR_LEFT + GE::Math::Vector3(486 / 2 - 32 * 7 / 2, 150, 0) + SCALE_NUMBER / 2;


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
		Draw(POS_CLEAR_TIME, SCALE_CLEAR_TIME, "ClearTime");
	}

	if (timer >= 1.0f) {
		std::string num = std::to_string(GameUtility::GetClearTime());
		DrawNum(num.substr(0, num.find(".") + 5), POS_NUMBER_LEFT, SCALE_NUMBER, 0);
	}
}

void Result::SendScore(float time)
{
	//�����L���ODB�Ƀf�[�^�o�^
	try {
		auto serverStatusCode = pplx::create_task([=]
			{
				//�N���C�A���g�̐ݒ�
				http_client client(BASE_URL + L"/score");

				//���M�f�[�^�̍쐬
				json::value postData;
				postData[L"score"] = json::value::number(time);

				//�g�[�N���ݒ�p
				http_request request;
				request.set_method(methods::POST);
				request.set_body(postData.serialize(), L"application/json");

				//���N�G�X�g���M
				return client.request(request);
			})
			.then([](http_response response) {
				//�X�e�[�^�X�R�[�h����
				if (response.status_code() == status_codes::OK) {
					//json��Ԃ�
					return response.extract_json();
				}
				})
				.then([](json::value json) {
					return json[L"serverStatus"].as_integer();
					}).wait();
	}
	catch (...) {
		return;
	}
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
		if (num[i] != '.') {
			Draw(pos + GE::Math::Vector3(i * scale.x + padding, 0, 0), scale, "Number_" + num.substr(i, 1));
		}
		else {
			Draw(pos + GE::Math::Vector3(i * scale.x + padding, 0, 0), scale, "Dot");
		}
	}
}
