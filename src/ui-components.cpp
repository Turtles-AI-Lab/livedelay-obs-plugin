/*
LiveDelay - UI Components Implementation
User interface for the plugin
*/

#include "ui-components.h"
#include <QGroupBox>
#include <QMessageBox>

LiveDelayDock::LiveDelayDock(QWidget *parent, LiveDelayManager *manager)
	: QDockWidget(parent), m_delayManager(manager)
{
	setWindowTitle("LiveDelay");
	setFeatures(QDockWidget::DockWidgetMovable |
		    QDockWidget::DockWidgetFloatable);

	setupUI();

	// Start update timer (updates status every second)
	m_updateTimer = new QTimer(this);
	connect(m_updateTimer, &QTimer::timeout, this,
		&LiveDelayDock::updateStatus);
	m_updateTimer->start(1000);

	updateStatus();
}

LiveDelayDock::~LiveDelayDock()
{
	if (m_updateTimer) {
		m_updateTimer->stop();
		delete m_updateTimer;
	}
}

void LiveDelayDock::setupUI()
{
	m_contentWidget = new QWidget(this);
	m_layout = new QVBoxLayout(m_contentWidget);
	m_layout->setSpacing(10);
	m_layout->setContentsMargins(10, 10, 10, 10);

	// Status Label (shows current delay state)
	m_statusLabel = new QLabel("Status: No delay (Live)", m_contentWidget);
	m_statusLabel->setStyleSheet(
		"QLabel { font-size: 14px; font-weight: bold; padding: 8px; "
		"background-color: #28a745; color: white; border-radius: 4px; }");
	m_statusLabel->setAlignment(Qt::AlignCenter);
	m_layout->addWidget(m_statusLabel);

	// Toggle Button (main action button)
	m_toggleButton = new QPushButton("Enable Tournament Mode",
					 m_contentWidget);
	m_toggleButton->setStyleSheet(
		"QPushButton { font-size: 13px; padding: 10px; background-color: #007bff; "
		"color: white; border: none; border-radius: 4px; } "
		"QPushButton:hover { background-color: #0056b3; } "
		"QPushButton:pressed { background-color: #004085; }");
	m_toggleButton->setMinimumHeight(40);
	connect(m_toggleButton, &QPushButton::clicked, this,
		&LiveDelayDock::onToggleDelay);
	m_layout->addWidget(m_toggleButton);

	// Preset Selection Group
	QGroupBox *presetGroup = new QGroupBox("Quick Presets", m_contentWidget);
	QVBoxLayout *presetLayout = new QVBoxLayout(presetGroup);

	m_presetCombo = new QComboBox(presetGroup);
	m_presetCombo->addItem("No Delay (Live)", DELAY_NONE);
	m_presetCombo->addItem("30 Seconds", DELAY_30_SEC);
	m_presetCombo->addItem("1 Minute", DELAY_1_MIN);
	m_presetCombo->addItem("2 Minutes (Tournament)", DELAY_2_MIN);
	m_presetCombo->addItem("5 Minutes (High Security)", DELAY_5_MIN);
	m_presetCombo->setCurrentIndex(3); // Default to 2 minutes
	connect(m_presetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
		this, &LiveDelayDock::onPresetChanged);
	presetLayout->addWidget(m_presetCombo);

	m_layout->addWidget(presetGroup);

	// Custom Delay Group
	QGroupBox *customGroup = new QGroupBox("Custom Delay", m_contentWidget);
	QVBoxLayout *customLayout = new QVBoxLayout(customGroup);

	QLabel *customLabel = new QLabel("Delay (seconds):", customGroup);
	customLayout->addWidget(customLabel);

	m_customDelaySpinBox = new QSpinBox(customGroup);
	m_customDelaySpinBox->setRange(0, 600); // 0 to 10 minutes
	m_customDelaySpinBox->setValue(120);
	m_customDelaySpinBox->setSuffix(" sec");
	connect(m_customDelaySpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
		this, &LiveDelayDock::onCustomDelayChanged);
	customLayout->addWidget(m_customDelaySpinBox);

	m_layout->addWidget(customGroup);

	// Info Label
	m_infoLabel = new QLabel(
		"💡 Toggle delay without stopping your stream!\n"
		"Perfect for tournaments and competitive play.",
		m_contentWidget);
	m_infoLabel->setWordWrap(true);
	m_infoLabel->setStyleSheet(
		"QLabel { font-size: 11px; color: #666; padding: 8px; "
		"background-color: #f8f9fa; border-radius: 4px; }");
	m_layout->addWidget(m_infoLabel);

	// Add spacer to push everything to the top
	m_layout->addStretch();

	setWidget(m_contentWidget);
}

void LiveDelayDock::onToggleDelay()
{
	if (m_delayManager->isDelayActive()) {
		// Disable delay
		if (m_delayManager->disableDelay()) {
			QMessageBox::information(
				this, "LiveDelay",
				"Delay removed! Stream is now live with no delay.");
		} else {
			QMessageBox::warning(
				this, "LiveDelay",
				"Failed to remove delay. Check OBS logs for details.");
		}
	} else {
		// Enable delay (use current preset/custom value)
		int delaySeconds = m_customDelaySpinBox->value();
		if (m_delayManager->enableDelay(delaySeconds)) {
			QMessageBox::information(
				this, "LiveDelay",
				QString("Tournament Mode enabled! %1 delay added.\n"
					"Your stream stays online, viewers stay watching!")
					.arg(m_delayManager->getStatusText()));
		} else {
			QMessageBox::warning(
				this, "LiveDelay",
				"Failed to enable delay. Make sure your stream is running.");
		}
	}

	updateStatus();
}

void LiveDelayDock::onPresetChanged(int index)
{
	int preset = m_presetCombo->currentData().toInt();
	if (preset > 0) {
		m_customDelaySpinBox->setValue(preset);
	} else {
		m_customDelaySpinBox->setValue(0);
	}
}

void LiveDelayDock::onCustomDelayChanged(int value)
{
	// Update preset combo to show "Custom" if value doesn't match presets
	bool matchesPreset = false;
	for (int i = 0; i < m_presetCombo->count(); i++) {
		if (m_presetCombo->itemData(i).toInt() == value) {
			m_presetCombo->setCurrentIndex(i);
			matchesPreset = true;
			break;
		}
	}

	if (!matchesPreset && value > 0) {
		// Could add a "Custom" option to the combo box here
	}
}

void LiveDelayDock::updateStatus()
{
	updateButtonState();
	updateStatusIndicator();
}

void LiveDelayDock::updateButtonState()
{
	if (m_delayManager->isDelayActive()) {
		m_toggleButton->setText("Disable Tournament Mode");
		m_toggleButton->setStyleSheet(
			"QPushButton { font-size: 13px; padding: 10px; background-color: #dc3545; "
			"color: white; border: none; border-radius: 4px; } "
			"QPushButton:hover { background-color: #c82333; } "
			"QPushButton:pressed { background-color: #bd2130; }");
	} else {
		m_toggleButton->setText("Enable Tournament Mode");
		m_toggleButton->setStyleSheet(
			"QPushButton { font-size: 13px; padding: 10px; background-color: #007bff; "
			"color: white; border: none; border-radius: 4px; } "
			"QPushButton:hover { background-color: #0056b3; } "
			"QPushButton:pressed { background-color: #004085; }");
	}

	m_toggleButton->setEnabled(!m_delayManager->isTransitioning());
}

void LiveDelayDock::updateStatusIndicator()
{
	const char *statusText = m_delayManager->getStatusText();
	m_statusLabel->setText(QString("Status: %1").arg(statusText));

	if (m_delayManager->isTransitioning()) {
		m_statusLabel->setStyleSheet(
			"QLabel { font-size: 14px; font-weight: bold; padding: 8px; "
			"background-color: #ffc107; color: black; border-radius: 4px; }");
	} else if (m_delayManager->isDelayActive()) {
		m_statusLabel->setStyleSheet(
			"QLabel { font-size: 14px; font-weight: bold; padding: 8px; "
			"background-color: #dc3545; color: white; border-radius: 4px; }");
	} else {
		m_statusLabel->setStyleSheet(
			"QLabel { font-size: 14px; font-weight: bold; padding: 8px; "
			"background-color: #28a745; color: white; border-radius: 4px; }");
	}

	// Update custom delay spinbox to show current delay
	if (m_delayManager->isDelayActive()) {
		m_customDelaySpinBox->setValue(
			m_delayManager->getCurrentDelay());
	}
}
