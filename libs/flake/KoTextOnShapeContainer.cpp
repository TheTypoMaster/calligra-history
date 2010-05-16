/* This file is part of the KDE project
 * Copyright (C) 2010 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KoTextOnShapeContainer.h"
#include "KoShapeContainer_p.h"
#include "SimpleShapeContainerModel.h"
#include "KoShapeRegistry.h"
#include "KoShapeFactoryBase.h"
#include "KoTextShapeDataBase.h"

#include <KDebug>

class KoTextOnShapeContainerPrivate : public KoShapeContainerPrivate
{
public:
    KoTextOnShapeContainerPrivate(KoShapeContainer *q);
    virtual ~KoTextOnShapeContainerPrivate();

    KoShape *content; // the original shape
    KoShape *textShape;
};

class KoTextOnShapeContainerModel : public SimpleShapeContainerModel
{
public:
    KoTextOnShapeContainerModel(KoTextOnShapeContainer *qq, KoTextOnShapeContainerPrivate *containerData);
    virtual void containerChanged(KoShapeContainer *container);
    virtual void proposeMove(KoShape *child, QPointF &move);
    virtual void childChanged(KoShape *child, KoShape::ChangeType type);

    KoTextOnShapeContainer *q;
    KoTextOnShapeContainerPrivate *containerData;
    bool lock;
};

// KoTextOnShapeContainerPrivate
KoTextOnShapeContainerPrivate::KoTextOnShapeContainerPrivate(KoShapeContainer *q)
    : KoShapeContainerPrivate(q),
    content(0),
    textShape(0)
{
}

KoTextOnShapeContainerPrivate::~KoTextOnShapeContainerPrivate()
{
    // the 'content' object is not owned by us
    delete textShape;
}

/// KoTextOnShapeContainerModel
KoTextOnShapeContainerModel::KoTextOnShapeContainerModel(KoTextOnShapeContainer *qq, KoTextOnShapeContainerPrivate *data)
    : q(qq),
    containerData(data),
    lock(false)
{
}

void KoTextOnShapeContainerModel::containerChanged(KoShapeContainer *container)
{
    if (lock)
        return;
    lock = true;
    Q_ASSERT(container == q);
    containerData->content->setSize(q->size());
    KoShape *text = containerData->textShape;
    if (text)
        text->setSize(q->size());
    lock = false;
}

void KoTextOnShapeContainerModel::proposeMove(KoShape *child, QPointF &move)
{
    if (child == containerData->textShape) { // not user movable
        move.setX(0);
        move.setY(0);
    }
}

void KoTextOnShapeContainerModel::childChanged(KoShape *child, KoShape::ChangeType type)
{
    if (lock)
        return;
    lock = true;
    // the container is leading in size, so the only reason we get here is when
    // one of the child shapes decided to resize itself. This would probably be
    // the text shape deciding it needs more space.
    KoShape *text = containerData->textShape;
    if (child == text) {
        switch (type) {
        case KoShape::SizeChanged:
            q->setSize(text->size()); // should have a policy to decide what to do
            break;
        default: // the others are not interesting for us
            break;
        }
    }
    lock = false;
}

/// KoTextOnShapeContainer
KoTextOnShapeContainer::KoTextOnShapeContainer(KoShape *childShape, KoResourceManager *documentResources)
    : KoShapeContainer(*(new KoTextOnShapeContainerPrivate(this)))
{
    Q_D(KoTextOnShapeContainer);
    Q_ASSERT(childShape);
    d->content = childShape;

    setPosition(childShape->position());
    setSize(childShape->size());
    setZIndex(childShape->zIndex());
    setTransformation(childShape->transformation());

    childShape->setPosition(QPointF()); // since its relative to my position, this won't move it
    childShape->setSelectable(false);

    d->model = new KoTextOnShapeContainerModel(this, d);
    addShape(childShape);

    QSet<KoShape*> delegates;
    delegates << childShape;
    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->get("TextShapeID");
    if (factory) { // not installed, thats too bad, but allowed
        d->textShape = factory->createDefaultShape(documentResources);
        Q_ASSERT(d->textShape); // would be a bug in the text shape;
        d->textShape->setSize(size());
        d->textShape->setPosition(childShape->position());
        d->textShape->setTransformation(childShape->transformation());
        KoTextShapeDataBase *shapeData = qobject_cast<KoTextShapeDataBase*>(d->textShape->userData());
        Q_ASSERT(shapeData); // would be a bug in kotext
        shapeData->setVerticalAlignment(Qt::AlignVCenter);
        addShape(d->textShape);
        d->textShape->setZIndex(childShape->zIndex() + 1);
        d->textShape->setSelectable(false);
        delegates << d->textShape;
    } else {
        kWarning(30006) << "Text shape factory not found";
    }
    setToolDelegates(delegates);
}

void KoTextOnShapeContainer::paintComponent(QPainter &, const KoViewConverter &)
{
}

bool KoTextOnShapeContainer::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    // TODO
    return false;
}

void KoTextOnShapeContainer::saveOdf(KoShapeSavingContext &context) const
{
    // TODO
}

void KoTextOnShapeContainer::setPlainText(const QString &text)
{
    Q_D(KoTextOnShapeContainer);
    if (d->textShape == 0) {
        kWarning(30006) << "No text shape present in KoTextOnShapeContainer";
        return;
    }
    KoTextShapeDataBase *shapeData = qobject_cast<KoTextShapeDataBase*>(d->textShape->userData());
    Q_ASSERT(shapeData->document());
    shapeData->document()->setPlainText(text);
}