//--------------------------------------------------------------------------------------
// File: SceneManager.h
//
// シーンを管理するクラス
// 
//
// Date: 2025.2.28
// 
// StackScene機能追加 2025.12.9
//--------------------------------------------------------------------------------------
#pragma once

// ESCキーで終了したい場合有効にしてください
#define ESC_QUIT_ENABLE

#ifdef ESC_QUIT_ENABLE
#include "Keyboard.h"
#endif

namespace Ito
{
	template <class T>
	class SceneManager;

	// シーンの基底クラス
	template <class T>
	class Scene
	{
	private:

		// シーンマネージャーへのポインタ
		SceneManager<T>* m_sceneManager;

	public:

		// コンストラクタ
		Scene() : m_sceneManager(nullptr) {

		}

		// デストラクタ
		virtual ~Scene() = default;

		// 初期化
		virtual void Initialize() = 0;

		// 更新
		virtual void Update(float elapsedTime) = 0;

		// 描画
		virtual void Render() = 0;

		// 終了処理
		virtual void Finalize() = 0;

		// デバイスに依存するリソースを作成する関数
		virtual void CreateDeviceDependentResources() {}

		// ウインドウサイズに依存するリソースを作成する関数
		virtual void CreateWindowSizeDependentResources() {}

		// デバイスロストした時に呼び出される関数
		virtual void OnDeviceLost() {}

	public:

		// シーンマネージャー設定関数
		void SetSceneManager(SceneManager<T>* sceneManager) { m_sceneManager = sceneManager; }

		// シーンの切り替え関数
		template <class U>
		void ChangeScene();

		// シーンの切り替え関数
		template <class U, typename... Args>
		void StackScene(Args&&... args);

		// スタックシーンの切り替え関数
		void DetachStackScene();


		// ユーザーが設定したリソース取得関数
		T* GetUserResources();

		void SetCurrentSceneTag(WIS::SceneTag tag) {
			m_sceneManager->SetCurrentSceneTag(tag);
		}


	};

	// シーンマネージャークラス
	template <class T>
	class SceneManager
	{
	private:

		// 共通でアクセスしたいオブジェクトへのポインタ
		T* m_userResources;

		// 実行中のシーンへのポインタ
		Scene<T>* m_scene;

		// 次のシーンへのポインタ
		Scene<T>* m_nextScene;



		//スタックするシーン
		Scene<T>* m_stackScene = nullptr;;

		//NextSceneとは別にスタックシーンを用意する
		Scene<T>* m_nextStackScene = nullptr;

		//スタックを消すためのフラグ
		bool m_deleteStackScene = false;

		// シーン削除関数
		void DeleteScene();

		// 現在のシーンタグ
		WIS::SceneTag m_CurrentSceneTag;



	public:

		// コンストラクタ
		SceneManager(T* userResources = nullptr)
			: m_userResources(userResources)
			, m_scene(nullptr)
			, m_nextScene(nullptr)
		{

			m_CurrentSceneTag = WIS::SceneTag::NONE;
		};

		// デストラクタ
		virtual ~SceneManager() { DeleteScene(); };

		// 更新
		void Update(float elapsedTime);

		// 描画
		void Render();

		// デバイスに依存するリソースを作成する関数
		void CreateDeviceDependentResources();

		// ウインドウサイズに依存するリソースを作成する関数
		void CreateWindowSizeDependentResources();

		// デバイスロストした時に呼び出される関数
		virtual void OnDeviceLost();

		// シーンの設定関数
		template <class U>
		void SetScene();

		// 次のシーンの設定関数
		template <class U>
		void SetNextScene();

		// スタックシーンの設定関数
		template <class U, typename... Args>
		void SetStackScene(Args&&... args);

		// スタックシーン削除関数
		void DeleteStackScene();


		// ユーザーリソース設定関数
		void SetUserResources(T* userResources) { m_userResources = userResources; }

		// ユーザーリソース取得関数
		T* GetUserResources() { return m_userResources; }

		//Sceneタグの取得
		WIS::SceneTag GetCurrentSceneTag() { return m_CurrentSceneTag; }

		void SetCurrentSceneTag(WIS::SceneTag tag) { m_CurrentSceneTag = tag; }


	};

	// シーンの切り替え関数
	template <class T>
	template <class U>
	void Scene<T>::ChangeScene()
	{
		m_sceneManager->SetNextScene<U>();
	}

	//シーンのスタック関数
	template <class T>
	template <class U, typename... Args>
	void Scene<T>::StackScene(Args&&... args)
	{
		m_sceneManager->SetStackScene<U>(std::forward<Args>(args)...);
	}

	template<class T>
	inline void Scene<T>::DetachStackScene()
	{
		m_sceneManager->DeleteStackScene();
	}

	// ユーザーが設定したリソース取得関数
	template <class T>
	T* Scene<T>::GetUserResources()
	{
		assert(m_sceneManager);

		return m_sceneManager->GetUserResources();
	}

	// シーンの設定関数
	template <class T>
	template <class U>
	void SceneManager<T>::SetScene()
	{
		// シーンを削除
		DeleteScene();

		assert(m_scene == nullptr);

		// シーンを生成
		m_scene = new U;

		// シーンにシーンマネージャーへのポインタを設定
		m_scene->SetSceneManager(this);

		// シーンの初期化処理
		m_scene->Initialize();


		//m_CurrentSceneTag->SetSceneTag(
	}

	// シーンの設定関数
	template <class T>
	template <class U>
	void SceneManager<T>::SetNextScene()
	{
		if (!m_nextScene)
		{
			// シーンを生成
			m_nextScene = new U;
		}
	}


	// スタックシーンの設定関数
	template <class T>
	template <class U, typename... Args>
	void SceneManager<T>::SetStackScene(Args&&... args)
	{
		if (!m_nextStackScene)
		{
			// シーンを生成し、引数をコンストラクタに渡す
			m_nextStackScene = new U(std::forward<Args>(args)...);
		}
	}

	// 更新関数
	template <class T>
	void SceneManager<T>::Update(float elapsedTime)
	{
#ifdef ESC_QUIT_ENABLE
		// ESCキーで終了
		auto kb = DirectX::Keyboard::Get().GetState();
		if (kb.Escape) PostQuitMessage(0);
#endif

		// シーンの切り替え処理
		if (m_nextScene)
		{
			DeleteScene();
			assert(m_scene == nullptr);
			m_scene = m_nextScene;
			m_nextScene = nullptr;
			m_scene->SetSceneManager(this);
			m_scene->Initialize();
		}

		//スタックシーンの切り替え処理
		if (m_nextStackScene)
		{
			m_stackScene = m_nextStackScene;
			m_nextStackScene = nullptr;
			m_stackScene->SetSceneManager(this);
			
			m_stackScene->Initialize();
		}

		// ★修正点: アクティブなシーンのみ更新する
		if (m_stackScene)
		{
			// スタックシーンが存在すれば、それだけを更新する
			m_stackScene->Update(elapsedTime);
		}
		else if (m_scene)
		{
			// スタックシーンがなければ、ベースのシーンを更新する
			m_scene->Update(elapsedTime);
		}
	}

	// 描画関数
	template <class T>
	void SceneManager<T>::Render()
	{
		// ベースシーンの描画
		if (m_scene) m_scene->Render();

		// スタックシーンの描画 (ベースの上に重ねる)
		if (m_stackScene) m_stackScene->Render();
	}


	// デバイスに依存するリソースを作成する関数
	template <class T>
	void SceneManager<T>::CreateDeviceDependentResources()
	{
		if (m_scene) m_scene->CreateDeviceDependentResources();
	}

	// ウインドウサイズに依存するリソースを作成する関数
	template <class T>
	void SceneManager<T>::CreateWindowSizeDependentResources()
	{
		if (m_scene) m_scene->CreateWindowSizeDependentResources();
	}

	// デバイスロストした時に呼び出される関数
	template <class T>
	void SceneManager<T>::OnDeviceLost()
	{
		if (m_scene) m_scene->OnDeviceLost();
	}

	// シーンの削除関数
	template <class T>
	void SceneManager<T>::DeleteScene()
	{
		if (m_scene)
		{
			m_scene->Finalize();
			delete m_scene;
			m_scene = nullptr;
		}
	}

	template<class T>
	inline void SceneManager<T>::DeleteStackScene()
	{
		if (m_stackScene)
		{
			m_stackScene->Finalize();
			delete m_stackScene;
			m_stackScene = nullptr;
		}
	}
}
