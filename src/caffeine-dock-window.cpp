
#include "caffeine-dock-window.hpp"
#include "instance.hpp"

#include <QString>
#include <QDesktopServices>
#include <QMessageBox>
#include <QPixmap>
#include <QFileDialog>

#include <filesystem>

#include "plugin-macros.generated.h"
#include "policy.hpp"

const int otpLength = 6;
static const char *getCaffeineURL = getenv("LIBCAFFEINE_DOMAIN") == NULL
					    ? "caffeine.tv"
					    : getenv("LIBCAFFEINE_DOMAIN");

CaffeineDockWindow::CaffeineDockWindow(QWidget *parent)
	: QDockWidget("Caffeine window", parent),
	  ui(new Ui::CaffeineDockWindow),
	  username_(""),
	  broadcastTitle_(caff::defaultTitle),
	  contentRating_("17+"),
	  thumbnailFilename_(""),
	  categoryMap_({})
{
	ui->setupUi(this);
	ui->messageLabel->clear();
	setVisible(false);

	setFloating(true);
	setMinimumSize(400, 500);

	loadSavedSettings();
	ui->updateButton->setVisible(false);
	ui->newVersionAvailableLabel->setVisible(false);

	// Add validator to allow only max 3 words separated by space or #
	ui->hashtagsLineEdit->setValidator(new QRegularExpressionValidator(
		QRegularExpression("^(?!.*#.*#)(\\s?#*\\w+){0,3}$"), this));

	// Set the version of the plugin
	ui->versionLabel->setText(QString::fromStdString(PLUGIN_VERSION));

	// Check for latest Version
	auto currentInstance = caff::Instance::getInstance();
	if (currentInstance->checkRecentVersion() == OldVersion) {
		ui->newVersionAvailableLabel->setVisible(true);
	}

	connect(ui->signInButton, SIGNAL(clicked()), this,
		SLOT(onSignInClicked()));

	// Hide MFA row
	ui->mfaLabel->setVisible(false);
	ui->mfaLineEdit->setVisible(false);
	ui->sendNewCodeBtn->setVisible(false);

	ui->forgotPasswordLabel->setText(
		QString(R"(<a href="https://www.%1/forgot-password" style="color: white; 
		text-decoration: none">Forgot Password</a>)")
			.arg(getCaffeineURL));
	// Only used for One-time-password "resend email" link. resending the
	// email is just attempting the sign-in without one-time password
	// included
	connect(ui->forgotPasswordLabel, &QLabel::linkActivated,
		[&](const QString &) { login(); });

	// Stream buttons
	connect(ui->openCaffeineButton, SIGNAL(clicked(bool)),
		SLOT(viewOnWebClicked(bool)));
	connect(ui->goLiveButton, SIGNAL(clicked()), this,
		SLOT(onGoLiveBtnClicked()));

	connect(ui->updateButton, SIGNAL(clicked()), this,
		SLOT(onUpdateButtonClicked()));

	connect(ui->sendNewCodeBtn, &QPushButton::clicked, [=]() {
		login();
		ui->messageLabel->clear();
		ui->messageLabel->setText(
			obs_module_text("Caffeine.Auth.SentCode"));
	});

	connect(ui->broadcastTitleLineEdit, &QLineEdit::textEdited,
		[&](const QString &text) { broadcastTitleTextChanged(text); });

	connect(ui->contentRating, SIGNAL(currentIndexChanged(int)), this,
		SLOT(onContentRatingChanged(int)));

	connect(ui->chooseThumbnailButton, SIGNAL(clicked()), this,
		SLOT(onChooseThumnbnailBtnClicked()));

	connect(ui->clearThumbnailButton, SIGNAL(clicked()), this,
		SLOT(onClearThumbnailBtnClicked()));
}

CaffeineDockWindow::~CaffeineDockWindow()
{
	if (obs_frontend_streaming_active()) {
		CAFF_LOG_DEBUG("Obs streaming  active ending broadcast");
		auto currentInstance = caff::Instance::getInstance();
		currentInstance->endBroadcast();

		obs_frontend_streaming_stop();
	}
	// Only save the settings when closing the app
	saveSettings();
	delete ui;
}

void CaffeineDockWindow::showErrorDialog(QString error)
{
	QWidget *obsWindow = (QWidget *)obs_frontend_get_main_window();
	QMessageBox::critical(obsWindow, "Error", error, QMessageBox::Ok);
}

void CaffeineDockWindow::login()
{
	auto currentInstance = caff::Instance::getInstance();
	std::string password = ui->passwordLineEdit->text().toStdString();
	std::string otp = ui->mfaLineEdit->text().toStdString();

	SessionAuthResponse result = currentInstance->signIn(
		ui->usernameLineEdit->text().toStdString().c_str(),
		password.c_str(), otp.c_str());

	ui->messageLabel->setVisible(true);
	// Hide MFA row
	ui->mfaLabel->setVisible(false);
	ui->mfaLineEdit->setVisible(false);
	ui->sendNewCodeBtn->setVisible(false);

	switch (result) {
	case Success:
		refreshToken_ = currentInstance->getRefreshToken();
		username_ = ui->usernameLineEdit->text().toStdString();
		// Hide username
		setupSignedInUI();
		return;
	case MfaOtpRequired:
		ui->messageLabel->setText(
			QString(R"(%1 <a href="https://www.%2" style="text-decoration:none;color:white;"></a>)")
				.arg(obs_module_text("Caffeine.Auth.EnterCode"),
				     getCaffeineURL));
		// Hide password row
		ui->passwordLabel->setVisible(false);
		ui->passwordLineEdit->setVisible(false);
		// Show MFA row
		ui->mfaLabel->setVisible(true);
		ui->mfaLineEdit->setVisible(true);
		ui->sendNewCodeBtn->setVisible(true);
		ui->forgotPasswordLabel->setVisible(false);
		ui->usernameLineEdit->setEnabled(false);

		ui->signInButton->setText(
			obs_module_text("Caffeine.Auth.Continue"));

		// Disable the signInButton upfront
		ui->signInButton->setEnabled(false);
		ui->mfaLineEdit->setMaxLength(otpLength);
		connect(ui->mfaLineEdit, &QLineEdit::textChanged,
			[=](const QString &enteredOtp) {
				if (enteredOtp.length() == otpLength) {
					ui->signInButton->setEnabled(true);
				} else {
					ui->signInButton->setEnabled(false);
				}
			});
		return;
	case MfaOtpIncorrect:
		// Show MFA row
		ui->mfaLabel->setVisible(true);
		ui->mfaLineEdit->setVisible(true);
		ui->sendNewCodeBtn->setVisible(true);
		ui->forgotPasswordLabel->setVisible(false);
		// Hide password row
		ui->passwordLabel->setVisible(false);
		ui->passwordLineEdit->setVisible(false);
		ui->mfaLineEdit->clear();
		ui->messageLabel->setText(
			obs_module_text("Caffeine.Auth.IncorrectVerification"));
		return;
	case UsernameRequired:
		ui->messageLabel->setText(
			obs_module_text("Caffeine.Auth.UsernameRequired"));
		return;
	case PasswordRequired:
		ui->messageLabel->setText(
			obs_module_text("Caffeine.Auth.PasswordRequired"));
		return;
	case InfoIncorrect:
		ui->messageLabel->setText(
			obs_module_text("Caffeine.Auth.IncorrectInfo"));
		return;
	case LegalAcceptanceRequired:
		ui->messageLabel->setText(
			obs_module_text("Caffeine.Auth.TosAcceptanceRequired"));
		return;
	case EmailVerificationRequired:
		ui->messageLabel->setText(obs_module_text(
			"Caffeine.Auth.EmailVerificationRequired"));
		return;
	case TooManyRequests:
		setupSignedOutUI();
		ui->messageLabel->setText(
			obs_module_text("Caffeine.Auth.TooManyRequests"));
		return;
	case InternetDisconnected:
		ui->messageLabel->setText(
			obs_module_text("Caffeine.InternetDisconnected.Text"));
		return;
	case Failure:
	default:
		ui->messageLabel->setText(
			obs_module_text("Caffeine.Auth.SigninFailed"));
		return;
	}
}

void CaffeineDockWindow::onSignInClicked()
{
	// Authenticate
	auto currentInstance = caff::Instance::getInstance();
	if (currentInstance->isSignedIn() ||
	    ui->signInButton->text() == "Sign out") {
		logout();
	} else {
		login();
	}
}

void CaffeineDockWindow::viewOnWebClicked(bool)
{
	if (username_.empty()) {
		return;
	}
	QUrl url;
	// Set the Caffeine URL default or staging
	std::string host = "www." + std::string(getCaffeineURL);
	url.setHost(QString::fromStdString(host));
	url.setPath(QString::fromStdString("/" + username_));
	url.setScheme("https");
	QDesktopServices::openUrl(url);
}

void CaffeineDockWindow::onGoLiveBtnClicked()
{
	// Disable the Button until it finishes the process
	ui->goLiveButton->setEnabled(false);
	ui->goLiveButton->setText(obs_module_text("Caffeine.Stream.Starting"));
	ui->goLiveButton->setStyleSheet(
		"QPushButton { text-align:left; border: 1px solid grey; border-radius: 8px; background-color:transparent; color: white; font-weight: bold; padding-left: 16px; }");

	ui->infoGroupBox->setEnabled(false);
	ui->signInButton->setEnabled(false);

	auto currentInstance = caff::Instance::getInstance();
	if (obs_frontend_streaming_active()) {
		obs_frontend_streaming_stop();
		currentInstance->endBroadcast();
		CAFF_LOG_INFO("Caffeine stream ended");
		clearSrtURL();
		setOfflineUI();
		return;
	}

	// Start caffeine streaming
	broadcastTitle_ = ui->broadcastTitleLineEdit->text().toStdString();

	std::vector<std::string> hashtags =
		caff::splitHashtags(ui->hashtagsLineEdit->text().toStdString());

	std::string category =
		ui->categoryComboBox->currentText().toStdString();

	// Start Streaming once srt url is
	if (!obs_frontend_streaming_active()) {
		auto *currentService = obs_frontend_get_streaming_service();
		const char *serviceName = obs_service_get_type(currentService);

		if (strcmp("rtmp_custom", serviceName) != 0) {
			setOfflineUI();
			showErrorDialog(QString::fromUtf8(obs_module_text(
				"Caffeine.Streaming.Service.Error")));
			CAFF_LOG_DEBUG("Current service not rtmp custom = %s",
				       serviceName);
			return;
		}

		if (currentInstance->checkMinSupportedVersion() ==
		    SessionAuthResponse::OldVersion) {
			setOfflineUI();
			showErrorDialog(QString::fromUtf8(obs_module_text(
				"Caffeine.Stream.ErrorOldVersion")));
			return;
		}

		std::string categoryHashtag = "";
		if (strcmp("None", category.c_str()) != 0) {
			categoryHashtag = categoryTag(category);
		}
		auto result = currentInstance->createSrtFeed(
			broadcastTitle_, username_, hashtags, categoryHashtag);
		auto error = std::get_if<SessionAuthResponse>(&result);

		if (error) {
			setOfflineUI();
			std::string errorMsg =
				std::string("Failed to create feed.") +
				sessionAuthResponseString(*error);
			showErrorDialog(QString::fromStdString(errorMsg));
			return;
		}

		std::string srtUrl_ = std::get<std::string>(result);
		if (srtUrl_.empty()) {
			setOfflineUI();
			showErrorDialog(QString::fromUtf8(obs_module_text(
				"Caffeine.Srt.Creation.Failure")));
			return;
		}

		setSrtURL(srtUrl_);

		// Trigger start streaming
		obs_frontend_streaming_start();
		auto startedCallback = [this]() { streamStarted(this); };
		auto failedCallback = [this](SessionAuthResponse error) {
			streamFailed(this, error);
		};
		currentInstance->startBroadcast(startedCallback,
						failedCallback);
	}
}

void CaffeineDockWindow::broadcastTitleTextChanged(const QString &text)
{
	QRegularExpression regex("\\#");

	if (regex.match(text).hasMatch()) {
		// Display warning
		ui->messageLabel->setText(obs_module_text(
			"Caffeine.ShowInfo.Title.HashtagWarning"));
		ui->messageLabel->setVisible(true);
	} else {
		ui->messageLabel->setText("");
		ui->messageLabel->setVisible(false);
	}
	// Check is broadcasting
	if (!ui->signInButton->isEnabled()) {
		ui->updateButton->setVisible(true);
	}
}

void CaffeineDockWindow::onContentRatingChanged(int index)
{
	Q_UNUSED(index);
	// Check is broadcasting
	if (!ui->signInButton->isEnabled()) {
		ui->updateButton->setVisible(true);
	}
}

void CaffeineDockWindow::onUpdateButtonClicked()
{
	// Do not update if not broadcasting.
	if (ui->signInButton->isEnabled()) {
		return;
	}
	broadcastTitle_ = ui->broadcastTitleLineEdit->text().toStdString();
	contentRating_ = ui->contentRating->currentText().toStdString();
	caffql::ContentRating rating =
		(strcmp("17+", contentRating_.c_str()) == 0)
			? caffql::ContentRating::SeventeenPlus
			: caffql::ContentRating::Everyone;
	auto currentInstance = caff::Instance::getInstance();
	bool updatedTitle = currentInstance->setBroadcastTitle(broadcastTitle_);
	bool updatedRating = currentInstance->setRating(rating);
	bool updatedThumbnail =
		currentInstance->setThumbnail(thumbnailFilename_);
	// Hide this after updated are sent to backend
	if (updatedTitle && updatedRating && updatedThumbnail) {
		ui->updateButton->setVisible(false);
	}
}

void CaffeineDockWindow::onChooseThumnbnailBtnClicked()
{
	QString thumbnailImageFile = QFileDialog::getOpenFileName(
		this, tr("Open Image"), "/home", tr("Image Files (*.jpg)"));

	if (!thumbnailImageFile.isEmpty()) {
		thumbnailFilename_ = thumbnailImageFile.toStdString();
		std::uintmax_t fileSize =
			std::filesystem::file_size(thumbnailFilename_);
		// Set thumbnail only if < 2Mb
		if (fileSize > (2 * 1024 * 1024)) {
			thumbnailFilename_ = "";
			showErrorDialog(QString::fromUtf8(obs_module_text(
				"Caffeine.ShowInfo.Thumbnail.Error")));
			return;
		}
		setThumbnailImage();

		// Show the update button if broadcasting
		if (!ui->signInButton->isEnabled()) {
			ui->updateButton->setVisible(true);
		}
	}
}

void CaffeineDockWindow::onClearThumbnailBtnClicked()
{
	// Clear previewImageLabel
	thumbnailFilename_ = "";
	ui->previewImageLabel->clear();
}

void CaffeineDockWindow::streamStarted(void *userData)
{
	QMutexLocker locker(&mutex_);
	CAFF_LOG_INFO("Caffeine stream started");
	auto *context = reinterpret_cast<CaffeineDockWindow *>(userData);
	context->contentRating_ =
		ui->contentRating->currentText().toStdString();
	caffql::ContentRating rating =
		(strcmp("17+", contentRating_.c_str()) == 0)
			? caffql::ContentRating::SeventeenPlus
			: caffql::ContentRating::Everyone;
	auto currentInstance = caff::Instance::getInstance();
	currentInstance->setRating(rating);
	currentInstance->setThumbnail(thumbnailFilename_);
	QMetaObject::invokeMethod(context, "setStreamingUI");
}

void CaffeineDockWindow::streamFailed(void *userData, SessionAuthResponse error)
{
	auto *context = reinterpret_cast<CaffeineDockWindow *>(userData);
	auto currentInstance = caff::Instance::getInstance();
	if (obs_frontend_streaming_active()) {
		obs_frontend_streaming_stop();
		currentInstance->endBroadcast();
		QMetaObject::invokeMethod(context, "clearSrtURL");
		QMetaObject::invokeMethod(context, "setOfflineUI");
	}
	CAFF_LOG_ERROR("Error streaming failed: %s",
		       sessionAuthResponseString(error));
	QMetaObject::invokeMethod(
		context, "showErrorDialog", Qt::QueuedConnection,
		Q_ARG(QString,
		      QString::fromUtf8(sessionAuthResponseString(error))));
}

void CaffeineDockWindow::setSrtURL(std::string srtUrl)
{
	auto *currentService = obs_frontend_get_streaming_service();

	if (!currentService) {
		CAFF_LOG_ERROR("No service found");
		return;
	}
	obs_data_t *settings = obs_service_get_settings(currentService);
	obs_data_set_string(settings, "server", srtUrl.c_str());
	obs_service_update(currentService, settings);

	obs_data_release(settings);
	obs_frontend_save_streaming_service();
}

void CaffeineDockWindow::clearSrtURL()
{
	auto *currentService = obs_frontend_get_streaming_service();
	if (!currentService) {
		CAFF_LOG_ERROR("No service found");
		return;
	}
	obs_data_t *settings = obs_service_get_settings(currentService);
	obs_data_set_string(settings, "server", "");
	obs_service_update(currentService, settings);
	obs_data_release(settings);
	obs_frontend_save_streaming_service();
}

void CaffeineDockWindow::setThumbnailImage()
{
	if (thumbnailFilename_.empty()) {
		return;
	}
	QPixmap thumbnailImage(QString::fromStdString(thumbnailFilename_));
	ui->previewImageLabel->setPixmap(thumbnailImage.scaled(
		ui->previewImageLabel->size(), Qt::KeepAspectRatio,
		Qt::SmoothTransformation));
}

std::string CaffeineDockWindow::categoryTag(std::string categoryTitle)
{
	auto it = categoryMap_.find(categoryTitle);
	if (it != categoryMap_.end()) {
		return it->second;
	}
	return "";
}

void CaffeineDockWindow::addCategories()
{
	ui->categoryComboBox->clear();
	ui->categoryComboBox->addItem("None");
	ui->categoryComboBox->insertSeparator(1);
	// Fetch category list
	auto currentInstance = caff::Instance::getInstance();
	std::vector<caff::CategoryResponse> categoryList =
		currentInstance->categories();
	for (auto it : categoryList) {
		categoryMap_[it.title] = it.tag;
		ui->categoryComboBox->addItem(QString::fromStdString(it.title));
	}
}

void CaffeineDockWindow::logout()
{
	auto currentInstance = caff::Instance::getInstance();
	currentInstance->signOut();
	username_ = "";

	setupSignedOutUI();
}

void CaffeineDockWindow::saveSettings()
{
	obs_data_t *obsData = obs_data_create();

	obs_data_set_string(obsData, "username", username_.c_str());
	if (username_ != "") {
		obs_data_set_string(obsData, "refresh_token",
				    refreshToken_.value_or("").c_str());
	} else {
		obs_data_set_string(obsData, "refresh_token", "");
	}

	obs_data_set_string(
		obsData, "content_rating",
		ui->contentRating->currentText().toStdString().c_str());
	obs_data_set_string(
		obsData, "title",
		ui->broadcastTitleLineEdit->text().toStdString().c_str());
	obs_data_set_string(obsData, "hashtags",
			    ui->hashtagsLineEdit->text().toStdString().c_str());
	obs_data_set_string(
		obsData, "category",
		ui->categoryComboBox->currentText().toStdString().c_str());
	obs_data_set_string(obsData, "thumbnail", thumbnailFilename_.c_str());

	char *file = obs_module_config_path(CONFIG);
	obs_data_save_json_safe(obsData, file, "tmp", "bak");
	obs_data_release(obsData);
	bfree(file);
}

void CaffeineDockWindow::setupSignedInUI()
{
	ui->usernameLabel->setVisible(false);
	ui->usernameLineEdit->setVisible(false);
	// Hide password row
	ui->passwordLabel->setVisible(false);
	ui->passwordLineEdit->setVisible(false);
	ui->messageLabel->setText("");
	ui->messageLabel->setVisible(false);
	ui->forgotPasswordLabel->setVisible(false);
	ui->signInButton->setEnabled(true);

	ui->signedInAs->setText(tr("%1").arg(username_.c_str()));
	ui->signInButton->setText("Sign out");

	// Hide MFA controls
	ui->mfaLabel->setVisible(false);
	ui->mfaLineEdit->setVisible(false);
	ui->sendNewCodeBtn->setVisible(false);

	// Enable streaming settings
	ui->infoGroupBox->setEnabled(true);
	ui->interactContents->setVisible(true);
	ui->goLiveButton->setEnabled(true);
	ui->infoGroupBox->setVisible(true);
	ui->goLiveButton->setVisible(true);
	ui->goLiveButton->setStyleSheet(
		"QPushButton { text-align:left; background-color:#0000ff; color: white; font-weight: bold; padding-left: 16px; }");
	addCategories();
}

void CaffeineDockWindow::setupSignedOutUI()
{
	ui->usernameLabel->setVisible(true);
	ui->usernameLineEdit->setVisible(true);
	// Hide password row
	ui->passwordLabel->setVisible(true);
	ui->passwordLineEdit->setVisible(true);
	ui->usernameLineEdit->setEnabled(true);
	ui->passwordLineEdit->clear();
	ui->usernameLineEdit->clear();
	ui->messageLabel->setVisible(true);
	ui->messageLabel->setText("");
	ui->forgotPasswordLabel->setVisible(true);
	ui->signInButton->setEnabled(true);

	ui->signedInAs->setText("");
	ui->signInButton->setText("Sign in");

	// Hide MFA controls
	ui->mfaLabel->setVisible(false);
	ui->mfaLineEdit->setVisible(false);
	ui->sendNewCodeBtn->setVisible(false);

	// Disable streaming settings
	ui->infoGroupBox->setEnabled(false);
	ui->interactContents->setVisible(false);
	ui->goLiveButton->setEnabled(false);

	ui->infoGroupBox->setVisible(false);
	ui->goLiveButton->setVisible(false);

	// Reset stream info
	ui->contentRating->setCurrentText("17+");
	ui->broadcastTitleLineEdit->clear();
	ui->previewImageLabel->clear();
	ui->categoryComboBox->clear();
	ui->hashtagsLineEdit->clear();
}

void CaffeineDockWindow::setOfflineUI()
{
	ui->signInButton->setEnabled(true);
	ui->infoGroupBox->setEnabled(true);
	ui->categoryComboBox->setEnabled(true);
	ui->categoryLabel->setEnabled(true);
	ui->hashtagsLineEdit->setEnabled(true);
	ui->hashtagsLabel->setEnabled(true);
	ui->clearThumbnailButton->setEnabled(true);
	ui->goLiveButton->setEnabled(true);
	ui->goLiveButton->setText(obs_module_text("Caffeine.Stream.GoLive"));
	ui->goLiveButton->setStyleSheet(
		"QPushButton { text-align:left; background-color:#0000ff; border-radius: 8px; color: white; font-weight: bold; padding-left: 16px; }");
}

void CaffeineDockWindow::setStreamingUI()
{
	// Disable sign out button when streaming
	ui->signInButton->setEnabled(false);
	ui->infoGroupBox->setEnabled(true);
	// Disable category and hashtags
	ui->categoryComboBox->setEnabled(false);
	ui->categoryLabel->setEnabled(false);
	ui->hashtagsLineEdit->setEnabled(false);
	ui->hashtagsLabel->setEnabled(false);

	ui->clearThumbnailButton->setEnabled(false);
	ui->goLiveButton->setEnabled(true);
	ui->goLiveButton->setText(obs_module_text("Caffeine.Stream.EndShow"));
	ui->goLiveButton->setStyleSheet(
		"QPushButton { text-align:left; background-color:red; border-radius: 8px; color: white; font-weight: bold; padding-left: 16px; }");
}

void CaffeineDockWindow::loadSavedSettings()
{
	char *file = obs_module_config_path(CONFIG);
	obs_data_t *obsData = nullptr;

	if (file) {
		obsData = obs_data_create_from_json_file(file);
		bfree(file);
	}
	if (obsData) {
		username_ = obs_data_get_string(obsData, "username");
		refreshToken_ = obs_data_get_string(obsData, "refresh_token");
		ui->contentRating->setCurrentText(
			obs_data_get_string(obsData, "content_rating"));
		ui->broadcastTitleLineEdit->setText(
			obs_data_get_string(obsData, "title"));
		broadcastTitleTextChanged(ui->broadcastTitleLineEdit->text());
		ui->hashtagsLineEdit->setText(
			obs_data_get_string(obsData, "hashtags"));

		if (refreshToken_ != std::nullopt ||
		    refreshToken_.value() != "") {
			auto currentInstance = caff::Instance::getInstance();
			currentInstance->refreshAuth(
				refreshToken_.value().c_str());
		}

		if (!username_.empty()) {
			setupSignedInUI();
		} else {
			setupSignedOutUI();
		}

		int categoryIndex = ui->categoryComboBox->findText(
			obs_data_get_string(obsData, "category"));
		if (categoryIndex >= 0) {
			ui->categoryComboBox->setCurrentIndex(categoryIndex);
		}
		thumbnailFilename_ = obs_data_get_string(obsData, "thumbnail");
		setThumbnailImage();
		obs_data_release(obsData);
	}
}
