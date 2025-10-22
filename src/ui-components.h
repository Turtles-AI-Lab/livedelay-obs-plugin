/*
LiveDelay - UI Components
Dock widget and controls for OBS interface
*/

#pragma once

#include <QDockWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QTimer>
#include "delay-manager.h"

class LiveDelayDock : public QDockWidget {
	Q_OBJECT

public:
	explicit LiveDelayDock(QWidget *parent, LiveDelayManager *manager);
	~LiveDelayDock();

private slots:
	void onToggleDelay();
	void onPresetChanged(int index);
	void onCustomDelayChanged(int value);
	void updateStatus();

private:
	// UI Components
	QWidget *m_contentWidget;
	QVBoxLayout *m_layout;

	QLabel *m_statusLabel;
	QPushButton *m_toggleButton;
	QComboBox *m_presetCombo;
	QSpinBox *m_customDelaySpinBox;
	QLabel *m_infoLabel;

	// Delay manager
	LiveDelayManager *m_delayManager;

	// Update timer
	QTimer *m_updateTimer;

	// Helper methods
	void setupUI();
	void updateButtonState();
	void updateStatusIndicator();
};
