#include "stdafx.h"

// General
#include "UIScrollBar.h"

UIScrollBar::UIScrollBar() : base() {
	value = 0.5;

	begunokPoint = VECTOR_ZERO;
	begunokYOffset = 0;
	begunokSize = VECTOR_ZERO;

	begunokSelected = false;
	begunokPressed = false;

	begunokBounds = VECTOR_ZERO;
	begunokBoundsSize = 0;
}

UIScrollBar::~UIScrollBar() {
	if (begunokImage != nullptr)
		delete begunokImage;
}

void UIScrollBar::Init(cvec2 _position, cvec2 _size) {
	auto buttonTexture = _TexturesMgr->Add("images/ui/buttons/slider_default.png");

	auto imageSize = vec2(buttonTexture->GetSize().x, buttonTexture->GetSize().y / 2);
	auto coeff = _size / imageSize;

	begunokYOffset = 5.0f * coeff.y;
	begunokSize = vec2(13.0f, 25.0f) * coeff.y;

	begunokBounds = vec2(8.0f, 200.0f) * coeff.x;
	begunokBoundsSize = begunokBounds.y - begunokBounds.x;

	begunokImage = new Image(buttonTexture, vec2(4, 40), vec2(15, 27));

	base::Init(_position, _size, new Image(buttonTexture, VECTOR_ZERO, imageSize));
}

void UIScrollBar::Render() {
	base::Render();

	begunokPoint = vec2(begunokBounds.x + value * begunokBoundsSize, begunokYOffset) - vec2(begunokSize.x / 2, 0);

	if (begunokSelected)
		_Render->RenderRectangle(GetPosition() + begunokPoint, begunokSize, false);

	_Render->RenderImage(GetPosition() + begunokPoint, begunokImage, begunokSize);
}

MOUSE_MOVED_(UIScrollBar) {
	begunokSelected = Utils::PointInRectangle(_mousePos, begunokPoint, begunokPoint + begunokSize);

	if (begunokPressed) {
		auto culledMouseX = _mousePos.x - begunokBounds.x;
		clamp(reinterpret_cast<int&>(culledMouseX), 0, begunokBoundsSize);
		value = static_cast<double>(culledMouseX) / static_cast<double>(begunokBoundsSize);
		clamp(value, 0.0, 1.0);
	}
}

void UIScrollBar::OnMouseLeaved() {
	begunokSelected = false;
}

MOUSE_PRESSED(UIScrollBar) {
	if (begunokSelected)
		begunokPressed = true;

	return true;
}

MOUSE_RELEASE(UIScrollBar) {
	begunokPressed = false;

	return true;
}

MOUSE_WHEEL(UIScrollBar) {
	value += static_cast<double>(_yoffset) / 100.0;
	clamp(value, 0.0, 1.0);

	return true;
}
