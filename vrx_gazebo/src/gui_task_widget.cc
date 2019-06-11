/*
 * Copyright (C) 2014 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
#include <sstream>
#include <gazebo/msgs/msgs.hh>
#include "vrx_gazebo/Task.h"
#include "vrx_gazebo/gui_task_widget.hh"

using namespace gazebo;

// Register this plugin with the simulator
GZ_REGISTER_GUI_PLUGIN(GUITaskWidget)

/////////////////////////////////////////////////
GUITaskWidget::GUITaskWidget()
  : GUIPlugin()
{
  // Set the frame background and foreground colors
  this->setStyleSheet(
      "QFrame { background-color : rgba(100, 100, 100, 255); color : white; }");

  // Create the main layout
  QHBoxLayout *mainLayout = new QHBoxLayout;

  // Create the frame to hold all the widgets
  QFrame *mainFrame = new QFrame();

  // Create the layout that sits inside the frame
  QHBoxLayout *frameLayout = new QHBoxLayout();

  QLabel *label = new QLabel(tr("Task Info:"));

  // Create a time label
  QLabel *timeLabel = new QLabel(tr("00:00:00.00"));

  // Add the label to the frame's layout
  frameLayout->addWidget(label);
  frameLayout->addWidget(timeLabel);
  connect(this, SIGNAL(SetTaskInfo(QString)),
      timeLabel, SLOT(setText(QString)), Qt::QueuedConnection);

  // Add frameLayout to the frame
  mainFrame->setLayout(frameLayout);

  // Add the frame to the main layout
  mainLayout->addWidget(mainFrame);

  // Remove margins to reduce space
  frameLayout->setContentsMargins(4, 4, 4, 4);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  this->setLayout(mainLayout);

  // Position and resize this widget
  this->move(200, 10);
  this->resize(200, 40);

  // Create a node for transportation
  this->node = transport::NodePtr(new transport::Node());
  this->node->Init("default");
  this->taskSub = this->node->Subscribe("/vrx/task/info",
      &GUITaskWidget::OnTaskInfo, this);
}

/////////////////////////////////////////////////
GUITaskWidget::~GUITaskWidget()
{
}

/////////////////////////////////////////////////
void GUITaskWidget::OnTaskInfo(const vrx_gazebo::Task::ConstPtr &_msg)
{
  std::ostringstream taskInfoStream;
  taskInfoStream.str("");
  taskInfoStream << "Task Name: " << _msg->name << "\n";
  taskInfoStream << "Task Stream: " << _msg->state << "\n";
  taskInfoStream << "Ready Time: " << this->FormatTime(_msg->ready_time) << "\n";
  taskInfoStream << "Running Time: " << this->FormatTime(_msg->running_time) << "\n";
  taskInfoStream << "Elapsed Time: " << this->FormatTime(_msg->elapsed_time) << "\n";
  taskInfoStream << "Remaining Time: " << this->FormatTime(_msg->remaining_time) << "\n";
  taskInfoStream << "Timed out: " << _msg->timed_out << "\n";
  taskInfoStream << "Score: " << _msg->score << "\n";

  this->SetTaskInfo(QString::fromStdString(taskInfoStream.str()));
}

/////////////////////////////////////////////////
std::string GUITaskWidget::FormatTime(const ros::Time &_duration) const
{
  std::ostringstream stream;
  unsigned int day, hour, min, sec, msec;

  stream.str("");

  sec = _duration.toSec();

  day = sec / 86400;
  sec -= day * 86400;

  hour = sec / 3600;
  sec -= hour * 3600;

  min = sec / 60;
  sec -= min * 60;

  msec = rint(_msg.nsec() * 1e-6);

  stream << std::setw(2) << std::setfill('0') << day << " ";
  stream << std::setw(2) << std::setfill('0') << hour << ":";
  stream << std::setw(2) << std::setfill('0') << min << ":";
  stream << std::setw(2) << std::setfill('0') << sec << ".";
  stream << std::setw(3) << std::setfill('0') << msec;

  return stream.str();
}