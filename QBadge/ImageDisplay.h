#ifndef IMAGE_DISPLAY_H
#define IMAGE_DISPLAY_H

#include <QWidget>
#include <QPixmap>

class ImageDisplay : public QWidget{

	Q_OBJECT


public:

	ImageDisplay(QWidget *parent = nullptr, const QPixmap& pixmap = QPixmap());

	void paintEvent(QPaintEvent* e);

	void setPixmap(const QPixmap& pixmap);

	QPixmap pixmap() const;

	void setAlignment(Qt::Alignment align);

	void setText(const QString& text);

	void clear();

private:

	QPixmap m_pixmap;
};

#endif
