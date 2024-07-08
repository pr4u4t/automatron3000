#include "ImageDisplay.h"
#include <QPainter>

ImageDisplay::ImageDisplay(QWidget* parent, const QPixmap& pixmap)
	: QWidget(parent)
	, m_pixmap(pixmap){
}

void ImageDisplay::paintEvent(QPaintEvent* e) {
	QPainter painter(this);
	QMargins margins = contentsMargins();
	QPixmap pixmap = m_pixmap.scaled(size(), Qt::KeepAspectRatio);
	int left = margins.left()+(size().width()-pixmap.size().width())/2;
	int top = margins.top()+ (size().height() - pixmap.size().height()) / 2;
	painter.drawPixmap(left, top, pixmap.width(), pixmap.height(), pixmap);
}

void ImageDisplay::setPixmap(const QPixmap& pixmap) {
	m_pixmap = pixmap;
}

QPixmap ImageDisplay::pixmap() const {
	return m_pixmap;
}

void ImageDisplay::setAlignment(Qt::Alignment align) {
	
}

void ImageDisplay::setText(const QString& text) {

}

void ImageDisplay::clear() {
	m_pixmap = QPixmap();
}