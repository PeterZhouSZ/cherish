#include "EditEntityCommand.h"

#include <QObject>

EditCanvasOffsetCommand::EditCanvasOffsetCommand(entity::UserScene *scene, const osg::Vec3f &translate, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(scene->getCanvasCurrent())
    , m_translate(translate)
{
    this->setText(QObject::tr("Offset %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

EditCanvasOffsetCommand::~EditCanvasOffsetCommand()
{
}

void EditCanvasOffsetCommand::undo()
{
    m_canvas->translate(osg::Matrix::translate(-m_translate.x(), -m_translate.y(), -m_translate.z()));
    m_scene->updateWidgets();
}

void EditCanvasOffsetCommand::redo()
{
    m_canvas->translate(osg::Matrix::translate(m_translate.x(), m_translate.y(), m_translate.z()));
    m_scene->updateWidgets();
}

/* =====================*/

EditCanvasRotateCommand::EditCanvasRotateCommand(entity::UserScene *scene, const osg::Quat &rotate, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(scene->getCanvasCurrent())
    , m_rotate(rotate)
{
    this->setText(QObject::tr("Rotate %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

EditCanvasRotateCommand::~EditCanvasRotateCommand()
{

}

void EditCanvasRotateCommand::undo()
{
    double angle;
    osg::Vec3d axis;
    m_rotate.getRotate(angle, axis);
    m_canvas->rotate(osg::Matrix::rotate(-angle, axis));
    m_scene->updateWidgets();
}

void EditCanvasRotateCommand::redo()
{
    m_canvas->rotate(osg::Matrix::rotate(m_rotate));
    m_scene->updateWidgets();
}



EditPhotoMoveCommand::EditPhotoMoveCommand(entity::UserScene *scene, const double u, const double v, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(scene->getCanvasCurrent())
    , m_photo(m_canvas->getPhotoCurrent())
    , m_u0(m_photo->getCenter().x())
    , m_v0(m_photo->getCenter().y())
    , m_u1(u)
    , m_v1(v)
{
    this->setText(QObject::tr("Move %1 within %2")
                  .arg(QString(m_photo->getName().c_str()),
                       QString(m_canvas->getName().c_str())));
}

EditPhotoMoveCommand::~EditPhotoMoveCommand()
{
}

void EditPhotoMoveCommand::undo()
{
    m_photo->move(m_u0, m_v0);
    m_canvas->updateFrame();
    m_scene->updateWidgets();
}

void EditPhotoMoveCommand::redo()
{
    m_photo->move(m_u1, m_v1);
    m_canvas->updateFrame();
    m_scene->updateWidgets();
}

EditStrokesPushCommand::EditStrokesPushCommand(entity::UserScene *scene, const std::vector<entity::Stroke *> &strokes,
                                               entity::Canvas *current, entity::Canvas *target,
                                               const osg::Vec3f &eye,
                                               QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_strokes(strokes)
    , m_canvasCurrent(current)
    , m_canvasTarget(target)
    , m_eye(eye)
{
    m_canvasCurrent->unselectStrokes();
    this->setText(QObject::tr("Push set of strokes from %1 to %2")
                  .arg(QString(m_canvasCurrent->getName().c_str()),
                       QString(m_canvasTarget->getName().c_str())));
}

void EditStrokesPushCommand::undo()
{
    this->doPushStrokes(*(m_canvasTarget.get()), *(m_canvasCurrent.get()));
    m_scene->updateWidgets();
}

void EditStrokesPushCommand::redo()
{
    this->doPushStrokes(*(m_canvasCurrent.get()), *(m_canvasTarget.get()));
    m_scene->updateWidgets();
}

void EditStrokesPushCommand::doPushStrokes(entity::Canvas& source, entity::Canvas& target)
{
    osg::Matrix M = source.getTransform()->getMatrix();
    osg::Matrix invM = osg::Matrix::inverse(target.getTransform()->getMatrix());

    const osg::Plane plane = target.getPlane();
    const osg::Vec3f center = target.getCenter();

    for (unsigned int i=0; i<m_strokes.size(); ++i){
        entity::Stroke* s = m_strokes.at(i);
        osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(s->getVertexArray());
        for (unsigned int j=0; j<verts->size(); ++j){
            osg::Vec3f p = (*verts)[j];
            osg::Vec3f P = p * M;
            osg::Vec3f dir = P - m_eye;
            double len = plane.dotProductNormal(center-P) / plane.dotProductNormal(dir);
            osg::Vec3f P_ = dir * len + P;
            osg::Vec3f p_ = P_ * invM;
            (*verts)[j] = osg::Vec3f(p_.x(), p_.y(), 0.f);
        }
        target.getGeodeData()->addDrawable(s);
        source.getGeodeData()->removeDrawable(s);

        verts->dirty();
        s->dirtyBound();
    }
    target.updateFrame();
}

EditPhotoFlipCommand::EditPhotoFlipCommand(entity::UserScene *scene, bool horizontal, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(m_scene->getCanvasCurrent())
    , m_photo(m_canvas->getPhotoCurrent())
    , m_horizontal(horizontal)
{
    QString str = horizontal? "Horizontal" : "Vertical";
    this->setText(QObject::tr("%1 Flip photo %2 within %3")
                  .arg(str,
                       QString(m_photo->getName().c_str()),
                       QString(m_canvas->getName().c_str())));
}

void EditPhotoFlipCommand::undo()
{
    if (m_horizontal)
        m_photo->flipH();
    else
        m_photo->flipV();
    m_scene->updateWidgets();
}

void EditPhotoFlipCommand::redo()
{
    if (m_horizontal)
        m_photo->flipH();
    else
        m_photo->flipV();
    m_scene->updateWidgets();
}

EditPhotoScaleCommand::EditPhotoScaleCommand(entity::UserScene *scene, const double scale, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(scene->getCanvasCurrent())
    , m_photo(m_canvas->getPhotoCurrent())
    , m_scale(scale)
{
    this->setText(QObject::tr("Scale %1 within %2")
                  .arg(QString(m_photo->getName().c_str()),
                       QString(m_canvas->getName().c_str())) );
}

void EditPhotoScaleCommand::undo()
{
    m_photo->scale(1.f/m_scale, 1.f/m_scale, m_photo->getCenter());
    m_canvas->updateFrame();
    m_scene->updateWidgets();
}

void EditPhotoScaleCommand::redo()
{
    m_photo->scale(m_scale, m_scale, m_photo->getCenter());
    m_canvas->updateFrame();
    m_scene->updateWidgets();
}

EditPhotoRotateCommand::EditPhotoRotateCommand(entity::UserScene *scene, const double angle, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(scene->getCanvasCurrent())
    , m_photo(m_canvas->getPhotoCurrent())
    , m_angle(angle)
{
    this->setText(QObject::tr("Rotate %1 within %2")
                  .arg(QString(m_photo->getName().c_str()),
                       QString(m_canvas->getName().c_str())) );
}

void EditPhotoRotateCommand::undo()
{
    m_photo->rotate(-m_angle);
    m_canvas->updateFrame();
    m_scene->updateWidgets();
}

void EditPhotoRotateCommand::redo()
{
    m_photo->rotate(m_angle);
    m_canvas->updateFrame();
    m_scene->updateWidgets();
}

EditCanvasDeleteCommand::EditCanvasDeleteCommand(entity::UserScene *scene, entity::Canvas *canvas, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(canvas)
{
this->setText(QObject::tr("Delete %1")
              .arg(QString(m_canvas->getName().c_str())));
}

void EditCanvasDeleteCommand::undo()
{
    emit m_scene->canvasAdded(m_canvas->getName());
    m_scene->addChild(m_canvas);
    m_scene->setCanvasCurrent(m_canvas);
    m_scene->updateWidgets();
}

void EditCanvasDeleteCommand::redo()
{
    if (m_canvas == m_scene->getCanvasCurrent())
        m_scene->setCanvasCurrent(m_scene->getCanvasPrevious());
    if (m_canvas == m_scene->getCanvasPrevious() ||
            m_scene->getCanvasCurrent() == m_scene->getCanvasPrevious()){
        for (unsigned int i = 0; i < m_scene->getNumChildren(); ++i){
            entity::Canvas* cnvi = dynamic_cast<entity::Canvas*>( m_scene->getChild(i));
            if (cnvi != NULL && cnvi != m_scene->getCanvasCurrent() && cnvi != m_canvas){
                m_scene->setCanvasPrevious(cnvi);
                break;
            }
        }
    }
    // now delete the canvas
    emit m_scene->canvasRemoved(m_scene->getCanvasIndex(m_canvas.get()));
    m_scene->removeChild(m_canvas);
    m_scene->updateWidgets();
}

EditStrokeDeleteCommand::EditStrokeDeleteCommand(entity::UserScene *scene, entity::Canvas *canvas, entity::Stroke *stroke, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(canvas)
    , m_stroke(stroke)
{
    this->setText(QObject::tr("Delete stroke from %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

void EditStrokeDeleteCommand::undo()
{
    m_canvas->getGeodeData()->addDrawable(m_stroke.get());
    m_scene->updateWidgets();
}

void EditStrokeDeleteCommand::redo()
{
    m_canvas->getGeodeData()->removeDrawable(m_stroke.get());
    m_scene->updateWidgets();
}

EditPhotoDeleteCommand::EditPhotoDeleteCommand(entity::UserScene *scene, entity::Canvas *canvas, entity::Photo *photo, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(canvas)
    , m_photo(photo)
{
this->setText(QObject::tr("Delete photo from %1")
              .arg(QString(m_canvas->getName().c_str())));
}

void EditPhotoDeleteCommand::undo()
{
    m_canvas->getGeodeData()->addDrawable(m_photo.get());
    m_scene->updateWidgets();
}

void EditPhotoDeleteCommand::redo()
{
    m_canvas->getGeodeData()->removeDrawable(m_photo.get());
    m_scene->updateWidgets();
}

EditStrokesMoveCommand::EditStrokesMoveCommand(entity::UserScene *scene, const std::vector<entity::Stroke *> &strokes, entity::Canvas *canvas, double du, double dv, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_strokes(strokes)
    , m_canvas(canvas)
    , m_du(du)
    , m_dv(dv)
{
    this->setText(QObject::tr("Move strokes within %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

void EditStrokesMoveCommand::undo()
{
    m_canvas->moveStrokes(m_strokes, -m_du, -m_dv);
    m_canvas->updateFrame();
    m_scene->updateWidgets();
}

void EditStrokesMoveCommand::redo()
{
    m_canvas->moveStrokes(m_strokes, m_du, m_dv);
    m_canvas->updateFrame();
    m_scene->updateWidgets();
}

EditStrokesScaleCommand::EditStrokesScaleCommand(entity::UserScene *scene, const std::vector<entity::Stroke *> &strokes, entity::Canvas *canvas, double scale, osg::Vec3f center, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_strokes(strokes)
    , m_canvas(canvas)
    , m_scale(scale)
    , m_center(center)
{
    this->setText(QObject::tr("Scale strokes within %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

void EditStrokesScaleCommand::undo()
{
    m_canvas->scaleStrokes(m_strokes, 1/m_scale, m_center);
    m_canvas->updateFrame();
    m_scene->updateWidgets();
}

void EditStrokesScaleCommand::redo()
{
    m_canvas->scaleStrokes(m_strokes, m_scale, m_center);
    m_canvas->updateFrame();
    m_scene->updateWidgets();
}

EditStrokesRotateCommand::EditStrokesRotateCommand(entity::UserScene *scene, const std::vector<entity::Stroke *> &strokes, entity::Canvas *canvas, double theta, osg::Vec3f center, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_strokes(strokes)
    , m_canvas(canvas)
    , m_theta(theta)
    , m_center(center)
{
    this->setText(QObject::tr("Rotate strokes within %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

void EditStrokesRotateCommand::undo()
{
    m_canvas->rotateStrokes(m_strokes, -m_theta, m_center);
    m_canvas->updateFrame();
    m_scene->updateWidgets();
}

void EditStrokesRotateCommand::redo()
{
    m_canvas->rotateStrokes(m_strokes, m_theta, m_center);
    m_canvas->updateFrame();
    m_scene->updateWidgets();
}

EditPasteCommand::EditPasteCommand(entity::UserScene *scene, entity::Canvas *target, const std::vector<osg::ref_ptr<entity::Stroke> > &buffer, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(target)
{
    for (size_t i=0; i<buffer.size(); ++i){
        const entity::Stroke& copy = *buffer.at(i);
        entity::Stroke* stroke = new entity::Stroke(copy, osg::CopyOp::DEEP_COPY_ALL);
        if (!stroke) continue;
        m_strokes.push_back(stroke);
    }

    this->setText(QObject::tr("Paste strokes to canvas %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

void EditPasteCommand::undo()
{
    m_canvas->unselectStrokes();
    for (size_t i=0; i<m_strokes.size();++i){
        m_canvas->getGeodeData()->removeDrawable(m_strokes.at(i));
    }
    m_canvas->updateFrame();
    m_scene->updateWidgets();
}

void EditPasteCommand::redo()
{
    m_canvas->unselectStrokes();
    for (size_t i=0; i<m_strokes.size(); ++i){
        entity::Stroke* stroke = m_strokes.at(i);
        if (!stroke) continue;
        stroke->moveDelta(0.2, 0.2);
        m_canvas->getGeodeData()->addDrawable(stroke);
        m_canvas->addStrokesSelected(stroke);
    }
    m_canvas->updateFrame();
    m_scene->updateWidgets();
}

EditCutCommand::EditCutCommand(entity::UserScene* scene, entity::Canvas*  canvas,
                               const std::vector<entity::Stroke*>& selected,
                               std::vector< osg::ref_ptr<entity::Stroke> >& buffer, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(canvas)
    , m_buffer(buffer)
    , m_selected(selected)
{
    this->setText(QObject::tr("Cutted strokes to buffer from canvas %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

void EditCutCommand::undo()
{
    m_scene->setCanvasCurrent(m_canvas.get());
    for (size_t i=0; i<m_buffer.size(); ++i){
        entity::Stroke* stroke = m_buffer.at(i);
        if (!stroke) continue;
        m_canvas->getGeodeData()->addDrawable(stroke);
        m_canvas->addStrokesSelected(stroke);
    }
    m_canvas->updateFrame();
    m_scene->updateWidgets();
}

void EditCutCommand::redo()
{
    for (size_t i=0; i<m_selected.size(); ++i){
        entity::Stroke* stroke = m_selected.at(i);
        if (!stroke) continue;
        m_buffer.push_back(stroke);
    }
    m_scene->setCanvasCurrent(m_canvas.get());
    for (size_t i=0; i<m_buffer.size(); ++i){
        m_canvas->getGeodeData()->removeDrawable(m_buffer.at(i));
    }
    m_canvas->updateFrame();
    m_scene->updateWidgets();
}

EditPhotoPushCommand::EditPhotoPushCommand(entity::UserScene *scene, entity::Canvas *current, entity::Canvas *previous, entity::Photo *photo, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_current(current)
    , m_previous(previous)
    , m_photo(photo)
{
    this->setText(QObject::tr("Move photo %1 from canvas %2 to canvas %3")
                  .arg(QString(m_photo->getName().c_str()) ,
                       QString(m_current->getName().c_str()),
                       QString(m_previous->getName().c_str()) ));
}

void EditPhotoPushCommand::undo()
{
    m_current->getGeodeData()->addDrawable(m_photo.get());
    //m_current->getGeodeData()->getOrCreateStateSet()->setTextureAttributeAndModes(0, m_photo->getTextureAsAttribute());
    m_previous->getGeodeData()->removeChild(m_photo.get());
    m_current->updateFrame();
    m_scene->updateWidgets();
}

void EditPhotoPushCommand::redo()
{
    m_previous->getGeodeData()->getOrCreateStateSet()->setTextureAttributeAndModes(0, m_photo->getTextureAsAttribute());
    m_previous->getGeodeData()->addDrawable(m_photo.get());
    m_current->getGeodeData()->removeDrawable(m_photo.get());
    m_previous->updateFrame();
    m_scene->updateWidgets();
}
