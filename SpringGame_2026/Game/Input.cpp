#include "Input.h"
#include "DxLib.h"

constexpr char kConfigfilename[] = "keyconf.kcnf";

namespace
{
	struct KeyConfHeader
	{
		//シグネチャ4バイト
		char signature[4];
		//バージョン番号4バイト
		float version; 
		//データ数4バイト
		int dataNum;
		//12バイト
	};
}
void Input::InitializeInputTable()
{
	m_inputTable["ok"] = { { PeripheralType::keyboard,KEY_INPUT_RETURN},
                      { PeripheralType::pad1,PAD_INPUT_B } };	// PADのAボタン
	m_inputTable["pause"] = { { PeripheralType::keyboard,KEY_INPUT_P},
                       { PeripheralType::pad1,PAD_INPUT_START } };	// PADのSTARTボタン

	m_inputTable["Dodge"] = { { PeripheralType::keyboard,KEY_INPUT_Z},
						  { PeripheralType::pad1,PAD_INPUT_A } };	// PADのAボタン
	//m_inputTable["slow"] = { { PeripheralType::keyboard,KEY_INPUT_LSHIFT},
	//					  { PeripheralType::pad1,PAD_INPUT_A } };	// PADのAボタン
	//m_inputTable["bomb"] = { { PeripheralType::keyboard,KEY_INPUT_X},
	//					  { PeripheralType::pad1,PAD_INPUT_X } };	// PADのYボタン

	m_inputTable["up"] = { { PeripheralType::keyboard,KEY_INPUT_UP},
						{ PeripheralType::pad1,PAD_INPUT_UP } };
	m_inputTable["down"] = { { PeripheralType::keyboard,KEY_INPUT_DOWN},
						  { PeripheralType::pad1,PAD_INPUT_DOWN } };
	m_inputTable["left"] = { { PeripheralType::keyboard,KEY_INPUT_LEFT},
						  { PeripheralType::pad1,PAD_INPUT_LEFT } };
	m_inputTable["right"] = { { PeripheralType::keyboard,KEY_INPUT_RIGHT},
						  { PeripheralType::pad1,PAD_INPUT_RIGHT } };
}

Input::Input(): m_inputData{},m_lastInputData{},m_inputTable{}
{
	InitializeInputTable();
	// 全イベントを false で初期化しておく（存在しないキーで at を呼ばないようにするため）
	for (const auto& kv : m_inputTable) {
		m_inputData[kv.first] = false;
		m_lastInputData[kv.first] = false;
	}
	Load();

	
	
}

void Input::Update()
{
	//まず現在の入力情報を取得
	char keyState[256];
	//生のキーボード情報
	GetHitKeyStateAll(keyState);
	//生のPad1を情報
	int padState = GetJoypadInputState(DX_INPUT_PAD1);
	//直前のフレームを更新(更新される前のフレーム情報をコピー)
	m_lastInputData = m_inputData;
	//全ての入力イベントをチェックする
	//ここでm_inputDataが更新される
	//m_inputTable回して各イベントの入力をチェックする
	for (const auto& inputInfo : m_inputTable)
	{
		auto& input = m_inputData[inputInfo.first];	// inputInfo.firstには"ok"等が入ってる
		// inputを書き換えると、inputData_のそのイベントが押されてるかどうかを着かえることになる
		// InputStateのvectorを回す
		for (const auto& state : inputInfo.second)
		{
			// このループをInputState配列のループなので、まず入力種別をチェックします
			switch (state.type)
			{
			case PeripheralType::keyboard:	// キーボードだったら
				// GetHitKeyStateAllで取ってきた配列の中身を見て入力されているかどうかチェック
				input = keyState[state.id];
				break;
			case PeripheralType::pad1:	// PADだったら
				// GetJoypadStateで取ってきたビット情報を見て登録されているビット情報と&を取り、そのビットが立っているかどうかをチェック
				input = (padState & state.id);
				break;
			}
			if (input)	// 必須！
			{
				// ここでbreakしないと、最後のチェックで押されてないとfalseになる
				break;
			}
		}
	}
}


	
	bool Input::IsPressed(const char* name)const
	{
		// もし「ない」イベントを送られるとクラッシュします
		return m_inputData.at(name);	// const関数内部なので[]ではなくatを使用してる
	}

	bool Input::IsTriggered(const char* name) const
	{
		return m_inputData.at(name) && !m_lastInputData.at(name);
	}

	//void Input::Save()
	//{
	//	// シグネチャ"kcnf"
	//	// バージョン番号
	//	// データ数
	//	// 先頭のデータサイズ固定部分を「ヘッダ」として書き込むために構造体を定義します
	//	FILE* fp = nullptr;
	//	auto err = fopen_s(&fp, "keyconf.kcnf", "wb");
	//	if (fp == nullptr)
	//	{
	//		return;
	//	}
	//	KeyConfHeader header = {};
	//	header.signature[0] = 'k';
	//	header.signature[1] = 'c';
	//	header.signature[2] = 'n';
	//	header.signature[3] = 'f';
	//	header.version = 1.0f;
	//	header.dataNum = m_inputTable.size();

	//	fwrite(&header, sizeof(header), 1, fp);	// 12バイトまるまる書き込んでいる

	//	// 個別のデータ
	//	for (const auto& info : m_inputTable)
	//	{
	//		const auto& name = info.first;
	//		byte nameLen = name.size();	// イベント名(文字列数)
	//		// 文字列を書き込む際は先頭1バイトを「文字列数」にすることをおすすめします
	//		fwrite(&nameLen, sizeof(nameLen), 1, fp);	// 文字サイズの書き込み
	//		fwrite(name.data(), nameLen, 1, fp);	// 文字データの書き込み

	//		const auto& data = info.second;
	//		byte dataNum = data.size();
	//		// データのサイズをあらかじめ書き込んでおく
	//		fwrite(&dataNum, sizeof(dataNum), 1, fp);
	//		// データをまとめて書き込む(サイズが分かっているから一気に書き込める)
	//		fwrite(data.data(), data.size() * sizeof(InputState), 1, fp);
	//	}

	//	fclose(fp);
	//}

	//void Input::Load()
	//{
	//	auto fileName = StringFunctions::WStringFromString(config_file_name);
	//	int handle = FileRead_open(fileName.c_str());
	//	KeyConfHeader header = {};
	//	FileRead_read(&header, sizeof(header), handle);
	//	for (int i = 0; i < header.dataNum; i++)
	//	{
	//		byte nameSize = 0;	// 名前の文字列数が分からないため、1バイトロード
	//		FileRead_read(&nameSize, sizeof(nameSize), handle);

	//		std::string name;	// イベント名受け取り用
	//		name.resize(nameSize);	// 受け取るために名前文字列の領域を確保
	//		// 確保した名前領域にセーブされているイベント名をコピーします
	//		FileRead_read(name.data(), name.size(), handle);
	//		// その名前がinputTable_にあるかどうかをチェック
	//		if (inputTable_.contains(name))
	//		{
	//			// もしあったら、そのテーブルデータを取得します
	//			auto& info = inputTable_.at(name);
	//			byte dataNum = 0;
	//			FileRead_read(&dataNum, sizeof(dataNum), handle);	// データ数を取得
	//			// 元のvectorサイズを超えないようにminで小さい方を選ぶようにする
	//			dataNum = std::min(static_cast<size_t>(dataNum), info.size());	// inputTable_[name]内のvectorをオーバーしないように
	//			// 必要な分だけデータをファイルからリードしてコピーします
	//			FileRead_read(info.data(), dataNum * sizeof(InputState), handle);
	//		}
	//	}
//	FileRead_close(handle);	// クローズを忘れないように

// 簡易実装: 永続化は未実装のため、リンカ用に空の定義を追加
void Input::Save()
{
	// 未実装
}

void Input::Load()
{
	// 未実装: 設定ファイル読み込みは行わない
}
