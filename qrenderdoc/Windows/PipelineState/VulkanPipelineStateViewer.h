/******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2017 Baldur Karlsson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

#pragma once

#include <QFrame>
#include "Code/CaptureContext.h"

namespace Ui
{
class VulkanPipelineStateViewer;
}

class RDTreeWidget;
class QTreeWidgetItem;
class PipelineStateViewer;

struct SamplerData
{
  SamplerData() : node(NULL) {}
  QList<QTreeWidgetItem *> images;
  QTreeWidgetItem *node;
};

class VulkanPipelineStateViewer : public QFrame, public ILogViewer
{
  Q_OBJECT

public:
  explicit VulkanPipelineStateViewer(ICaptureContext &ctx, PipelineStateViewer &common,
                                     QWidget *parent = 0);
  ~VulkanPipelineStateViewer();

  void OnLogfileLoaded();
  void OnLogfileClosed();
  void OnSelectedEventChanged(uint32_t eventID) {}
  void OnEventChanged(uint32_t eventID);

private slots:
  // automatic slots
  void on_showDisabled_toggled(bool checked);
  void on_showEmpty_toggled(bool checked);
  void on_exportHTML_clicked();
  void on_meshView_clicked();
  void on_viAttrs_itemActivated(QTreeWidgetItem *item, int column);
  void on_viBuffers_itemActivated(QTreeWidgetItem *item, int column);
  void on_viAttrs_mouseMove(QMouseEvent *event);
  void on_viBuffers_mouseMove(QMouseEvent *event);
  void on_pipeFlow_stageSelected(int index);

  // manual slots
  void shaderView_clicked();
  void shaderEdit_clicked();

  void shaderSave_clicked();
  void resource_itemActivated(QTreeWidgetItem *item, int column);
  void ubo_itemActivated(QTreeWidgetItem *item, int column);
  void vertex_leave(QEvent *e);

private:
  Ui::VulkanPipelineStateViewer *ui;
  ICaptureContext &m_Ctx;
  PipelineStateViewer &m_Common;

  QVariantList makeSampler(const QString &bindset, const QString &slotname,
                           const VKPipe::BindingElement &descriptor);
  void addResourceRow(ShaderReflection *shaderDetails, const VKPipe::Shader &stage, int bindset,
                      int bind, const VKPipe::Pipeline &pipe, RDTreeWidget *resources,
                      QMap<ResourceId, SamplerData> &samplers);
  void addConstantBlockRow(ShaderReflection *shaderDetails, const VKPipe::Shader &stage,
                           int bindset, int bind, const VKPipe::Pipeline &pipe, RDTreeWidget *ubos);

  void setShaderState(const VKPipe::Shader &stage, const VKPipe::Pipeline &pipe, QLabel *shader,
                      RDTreeWidget *res, RDTreeWidget *ubo);
  void clearShaderState(QLabel *shader, RDTreeWidget *res, RDTreeWidget *ubo);
  void setState();
  void clearState();

  void setInactiveRow(QTreeWidgetItem *node);
  void setEmptyRow(QTreeWidgetItem *node);
  void highlightIABind(int slot);

  QString formatMembers(int indent, const QString &nameprefix,
                        const rdctype::array<ShaderConstant> &vars);
  const VKPipe::Shader *stageForSender(QWidget *widget);

  QString disassembleSPIRV(const ShaderReflection *shaderDetails);

  template <typename viewType>
  void setViewDetails(QTreeWidgetItem *node, const viewType &view, TextureDescription *tex);

  template <typename viewType>
  void setViewDetails(QTreeWidgetItem *node, const viewType &view, BufferDescription *buf);

  bool showNode(bool usedSlot, bool filledSlot);

  // keep track of the VB nodes (we want to be able to highlight them easily on hover)
  QList<QTreeWidgetItem *> m_VBNodes;
  QList<QTreeWidgetItem *> m_BindNodes;

  // from an combined image to its sampler (since we de-duplicate)
  QMap<QTreeWidgetItem *, QTreeWidgetItem *> m_CombinedImageSamplers;
};
