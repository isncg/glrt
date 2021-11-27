#include "../../include/GLRT.h"
#include "../../utils/utils.h"
#include "../../utils/HL/HL.h"

namespace example
{
	class HalfLife1 :public Window
	{
		Camera						m_Camera;
		CameraFirstPersonController m_CamController;
		Model						m_LevelModel;
		Shader						m_LevelShader;
		Texture						m_LevelPlaceHolderTexture;
		std::vector<MeshRenderer>	m_LevelSubmeshRenderers;	//VAOs for each submesh of bsp map model
		std::vector<Material>		m_LevelSubmeshMaterials;	//material for each submesh of bsp map model
		RenderPass					m_LevelPass;
		RenderPass                  m_PostEffectPass;
		Shader						m_PostEffectShader;
		CanvasRenderer				m_PostEffectRenderer;
		DirectionalLight			m_ShadowMappingLight;
		Shader						m_ShadowMappingShader;
		long w = 1280, h = 720;

		void GetInitSize(long* width, long* height) override                { *width = w; *height = h; }
		virtual void OnMouseMove(long dx, long dy, long x, long y) override { m_CamController.OnMouseMove(dx, dy, x, y); }
		virtual void OnKeyboard(KEYS key, KEYACTION action) override        { m_CamController.OnKeyboard(key, action); }

		virtual void OnCreate() override
		{
			Window::OnCreate();
			Vector3 lightPos{ -500,2000,1000 };
			Vector3 lightDir{ 1,-4,-2 };
			std::map<std::string, Texture> textureDict;
			float lightRange = 4096;

			HLMapLoad(GetAssetPath("de_dust2.bsp"), GetAssetPath("cs_dust.wad"), m_LevelModel, &textureDict);
			LoadTexture(&m_LevelPlaceHolderTexture, "assets/256.bmp");

			m_ShadowMappingLight.InitLightMap(8192, 8192);
			m_ShadowMappingLight.SetLight(lightPos, lightDir, lightRange);
			m_ShadowMappingShader.Load(GetAssetPath("glsl/shadowmapping.vert"), GetAssetPath("glsl/shadowmapping.frag"));
			m_ShadowMappingShader.Set("lightmat", m_ShadowMappingLight.matrix);

			m_Camera.SetProjectionMatrix(3.1415926 / 3, (float)w / (float)h, 1, 5000);
			m_CamController.camera = &m_Camera;
			m_LevelShader.Load(GetAssetPath("glsl/bsplevel.vert"), GetAssetPath("glsl/bsplevel.frag"));
			m_LevelShader.Set("shadowmap", m_ShadowMappingLight.m_ShadowMappingPass.depthBuffer);
			m_LevelShader.Set("lightmat", m_ShadowMappingLight.matrix);
			m_LevelPass.Init(4096, 2048, 1, true);

			for (int i = 0; i < m_LevelModel.meshCollection.size(); i++)
			{
				MeshRenderer mr;
				m_LevelSubmeshRenderers.push_back(mr);
				m_LevelSubmeshRenderers[i].Set(&m_LevelModel.meshCollection[i]);

				Material mt;
				m_LevelSubmeshMaterials.push_back(mt);
				m_LevelSubmeshMaterials[i].Set(&m_LevelShader);

				auto texi = textureDict.find(m_LevelModel.matNames[i]);
				if (texi != textureDict.end())
					m_LevelSubmeshMaterials[i].Set("tex", texi->second);
				else
				{
					m_LevelSubmeshMaterials[i].Set("tex", m_LevelPlaceHolderTexture);
					std::cout << "Missing texture: " << m_LevelModel.matNames[i] << std::endl;
				}
			}

			for (int i = 0; i < m_LevelModel.meshCollection.size(); i++)
				m_LevelSubmeshRenderers[i].material = &m_LevelSubmeshMaterials[i];

			m_LevelModel.Clear();

			m_PostEffectRenderer.SetFullScreen();
			m_PostEffectShader.Load(GetAssetPath("glsl/posteffect.vert"), GetAssetPath("glsl/posteffect.frag"));
			m_PostEffectShader.Set("tex", m_LevelPass.colorBuffers[0]);
			glEnable(GL_DEPTH_TEST);
		}

		virtual void BeforeRender() override
		{
			m_ShadowMappingLight.m_ShadowMappingPass.Bind();
			m_ShadowMappingLight.m_ShadowMappingPass.Draw(m_LevelSubmeshRenderers, m_ShadowMappingShader);

			m_LevelShader.Set("_mvp", m_CamController.Update().GetCameraMatrix());
			m_LevelPass.Bind();
			m_LevelPass.Draw(m_LevelSubmeshRenderers);
		}

		virtual void Render() override
		{
			m_PostEffectPass.Draw(m_PostEffectRenderer, m_PostEffectShader);
		}
	};
}

RUN_WINDOWEX(example::HalfLife1, "examples/halflife1/assets/")