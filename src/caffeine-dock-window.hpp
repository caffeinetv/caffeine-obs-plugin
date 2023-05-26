#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QEvent>
#include <QObject>

#include <string>
#include <optional>
#include <unordered_map>

#include <util/base.h>
#include <util/platform.h>
#include <util/config-file.h>

#include <obs.h>
#include <obs.hpp>
#include <obs-frontend-api.h>
#include <obs-module.h>
#include <QMutex>
#include <QMutexLocker>

#include "caffeine.hpp"

#include "ui_CaffeineDockWindow.h"

#define CONFIG "config.json"

class CaffeineDockWindow : public QDockWidget {
	Q_OBJECT
public:
	explicit CaffeineDockWindow(QWidget *parent = nullptr);
	~CaffeineDockWindow();

private:
	std::optional<std::string> refreshToken_;
	Ui::CaffeineDockWindow *ui;
	std::string username_;
	std::string broadcastTitle_;
	std::string contentRating_;
	std::string thumbnailFilename_;
	std::unordered_map<std::string, std::string> categoryMap_;

	void saveSettings();
	void setupSignedInUI();
	void setupSignedOutUI();
	void loadSavedSettings();
	void login();
	void logout();
	void streamFailed(void *userData, SessionAuthResponse error);
	void streamStarted(void *userData);
	void setSrtURL(std::string srtUrl_);
	void setThumbnailImage();
	std::string categoryTag(std::string categoryTitle);
	void addCategories();
	QMutex mutex_;

private slots:
	void onSignInClicked();
	void viewOnWebClicked(bool);
	void onGoLiveBtnClicked();
	void showErrorDialog(QString error);
	void setStreamingUI();
	void setOfflineUI();
	void clearSrtURL();
	void broadcastTitleTextChanged(const QString &text);
	void onUpdateButtonClicked();
	void onContentRatingChanged(int index);
	void onChooseThumnbnailBtnClicked();
	void onClearThumbnailBtnClicked();
};
