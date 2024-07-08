#pragma once
#include "afx.h"
#include <glm.hpp>
#include "Camera.h"
#include "PrismMesh.h"
#include "Shader.h"
#include "IDragAcceptor.h"

class Poly2D;
struct ExtrudeParameters;

enum
{
	POSITION_BUFFER_INDEX,
	NORMAL_BUFFER_INDEX,
	EBO_INDEX,
	UV_BUFFER_INDEX,
};

class PrismView3DStatic : public CStatic, public IDragAcceptor
{
public:
	PrismView3DStatic();
	~PrismView3DStatic();
	virtual BOOL Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff) override;
	virtual void Serialize(CArchive& ar) override;

	void SetMesh(const Poly2D& mesh, const ExtrudeParameters& params);
	const PrismMesh& GetMesh() const { return m_Mesh; }
	void SetMeshInternal();
	void SetTexelsPerUnit(float newVal);

	// Inherited via IDragAcceptor
	virtual void GetWindowRect(LONG& t, LONG& l, LONG& b, LONG& r) const override;
	virtual void StartDrag(const glm::vec2& pt, MouseButton btn) override;
	virtual void StopDrag(const glm::vec2& pt) override;
	virtual void UpdateDrag(const glm::vec2& lastPt) override;
	
	bool IsMeshSet() { return m_bMeshSet; }
	void Clear();
private:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC*);

	void DeleteOpenGLContext();
	void CreateOpenGLContext();
	void SetProjectionMatrix();
	void DrawMesh();

	void SetLight(const glm::vec3& position, const glm::vec3& colour);
	void SetMaterial(const glm::vec3& objectColour, float ambientStrength, float diffuseStrength, float specularStrength, float shininess);
	void SetMeshModelMatrix(const glm::vec3& position);
	void InitRenderer();

private:
	Camera m_Cam;
	HGLRC m_hOpenGlCtx = NULL;
	glm::mat4x4 m_ProjectionMatrix;
	PrismMesh m_Mesh;
	Shader m_MeshShader;
	bool m_bMeshSet = false;
	glm::vec2 m_LastDragPos;

	unsigned int m_GLBuffers[4];
	unsigned int m_VAO = 0;
	unsigned int m_Texture = -1;

	float m_ViewportWidth;
	float m_ViewportHeight;
private:
	DECLARE_MESSAGE_MAP()
protected:
	DECLARE_SERIAL(PrismView3DStatic)
};

