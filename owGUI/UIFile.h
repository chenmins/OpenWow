#pragma once

// FORWARD BEGIN
class UIMgr;
// FORWARD END

class UIFile
{
public:
	UIFile(IUIMgr* _uimgr);

	bool Load(cstring _filename);
	void Destroy();

	UIWindow* GetRootWindow() const { return rootElement; }

	bool IsElementExists(cstring _name) const;
	UIElement* operator[](cstring _name);

	void SetParent(UIWindow* _parent);
	void Show() const;
	void Hide() const;

private:
	bool ProcessXMLNode(XMLNode* _node, UIElement* _parent);
	UIElement* LoadUIElement(XMLNode* _node);
	bool CreateUIElementByXMLNode(UIElement*& _element, XMLNode* _node);
	bool LoadCommonParams(UIElement*& _element, XMLNode* _node);

	Image* GetImage(XMLNode* _node);

private:
	std::string filename;

	UIWindow* parent;
	UIWindow* rootElement;

	map<std::string, UIElement*> elements;
	IUIMgr*		mUIMgr;
};
