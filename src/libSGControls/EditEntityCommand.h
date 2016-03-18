#ifndef EDITENTITYCOMMAND_H
#define EDITENTITYCOMMAND_H

#include <vector>

#include <QUndoCommand>

#include <osg/observer_ptr>

#include "UserScene.h"
#include "Canvas.h"
#include "Photo.h"
#include "Stroke.h"

class EditCanvasOffsetCommand : public QUndoCommand
{
public:
    EditCanvasOffsetCommand(entity::UserScene* scene, const osg::Vec3f& translate, QUndoCommand* parent = 0);
    ~EditCanvasOffsetCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::Vec3f m_translate;
};

class EditCanvasRotateCommand : public QUndoCommand
{
public:
    EditCanvasRotateCommand(entity::UserScene* scene, const osg::Quat& rotate, QUndoCommand* parent = 0);
    ~EditCanvasRotateCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::Quat m_rotate;
};

class EditCanvasDeleteCommand : public QUndoCommand
{
public:
    EditCanvasDeleteCommand(entity::UserScene* scene, entity::Canvas* canvas, QUndoCommand* parent = 0);
    ~EditCanvasDeleteCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::ref_ptr<entity::Canvas> m_canvas;
};

class EditPhotoMoveCommand : public QUndoCommand
{
public:
    EditPhotoMoveCommand(entity::UserScene* scene, const double u, const double v, QUndoCommand* parent = 0);
    ~EditPhotoMoveCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::observer_ptr<entity::Photo> m_photo;
    double m_u0, m_v0, m_u1, m_v1;
};

class EditPhotoScaleCommand : public QUndoCommand
{
public:
    EditPhotoScaleCommand(entity::UserScene* scene, const double scale, QUndoCommand* parent = 0);
    ~EditPhotoScaleCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::observer_ptr<entity::Photo> m_photo;
    double m_scale;
};

class EditPhotoFlipCommand : public QUndoCommand
{
public:
    EditPhotoFlipCommand(entity::UserScene* scene, bool horizontal, QUndoCommand* parent = 0);
    ~EditPhotoFlipCommand(){}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::observer_ptr<entity::Photo> m_photo;
    bool m_horizontal;
};

class EditPhotoRotateCommand : public QUndoCommand
{
public:
    EditPhotoRotateCommand(entity::UserScene* scene, const double angle, QUndoCommand* parent = 0);
    ~EditPhotoRotateCommand(){}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::observer_ptr<entity::Photo> m_photo;
    double m_angle;
};

class EditPhotoDeleteCommand : public QUndoCommand
{
public:
    EditPhotoDeleteCommand(entity::UserScene* scene, entity::Canvas* canvas, entity::Photo* photo, QUndoCommand* parent = 0);
    ~EditPhotoDeleteCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::ref_ptr<entity::Photo> m_photo;
};

class EditStrokesPushCommand : public QUndoCommand
{
public:
    EditStrokesPushCommand(entity::UserScene* scene, const std::vector<entity::Stroke*>& strokes, entity::Canvas* current, entity::Canvas* target,
                           const osg::Vec3f& eye, QUndoCommand* parent = 0);
    ~EditStrokesPushCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:

    void doPushStrokes(entity::Canvas& source, entity::Canvas& target);

    osg::observer_ptr<entity::UserScene> m_scene;
    const std::vector<entity::Stroke*> m_strokes;
    osg::observer_ptr<entity::Canvas> m_canvasCurrent;
    osg::observer_ptr<entity::Canvas> m_canvasTarget;
    osg::Vec3f m_eye;
};

class EditStrokesMoveCommand : public QUndoCommand
{
public:
    EditStrokesMoveCommand(entity::UserScene* scene, const std::vector<entity::Stroke*>& strokes, entity::Canvas* canvas,
                           double du, double dv, QUndoCommand* parent = 0);
    ~EditStrokesMoveCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    std::vector<entity::Stroke*> m_strokes;
    osg::observer_ptr<entity::Canvas> m_canvas;
    double m_du;
    double m_dv;
};

class EditStrokesScaleCommand : public QUndoCommand
{
public:
    EditStrokesScaleCommand(entity::UserScene* scene, const std::vector<entity::Stroke*>& strokes, entity::Canvas* canvas,
                            double scale, osg::Vec3f center, QUndoCommand* parent = 0);
    ~EditStrokesScaleCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    std::vector<entity::Stroke*> m_strokes;
    osg::observer_ptr<entity::Canvas> m_canvas;
    double m_scale;
    osg::Vec3f m_center;
};

class EditStrokesRotateCommand : public QUndoCommand
{
public:
    EditStrokesRotateCommand(entity::UserScene* scene, const std::vector<entity::Stroke*>& strokes, entity::Canvas* canvas,
                             double theta, osg::Vec3f center, QUndoCommand* parent = 0);
    ~EditStrokesRotateCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    std::vector<entity::Stroke*> m_strokes;
    osg::observer_ptr<entity::Canvas> m_canvas;
    double m_theta;
    osg::Vec3f m_center;
};

class EditStrokeDeleteCommand : public QUndoCommand
{
public:
    EditStrokeDeleteCommand(entity::UserScene* scene, entity::Canvas* canvas, entity::Stroke* stroke, QUndoCommand* parent = 0);
    ~EditStrokeDeleteCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    osg::ref_ptr<entity::Stroke> m_stroke;
};

class EditPasteCommand : public QUndoCommand
{
public:
    EditPasteCommand(entity::UserScene* scene, entity::Canvas* target,
                     const std::vector< osg::ref_ptr<entity::Stroke> >& buffer, QUndoCommand* parent=0);
    ~EditPasteCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    std::vector<entity::Stroke*> m_strokes;
};

class EditCutCommand : public QUndoCommand
{
public:
    EditCutCommand(entity::UserScene* scene, entity::Canvas*  canvas,
                   const std::vector<entity::Stroke*>& selected,
                   std::vector< osg::ref_ptr<entity::Stroke> >& buffer, QUndoCommand* parent=0);
    ~EditCutCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_canvas;
    std::vector< osg::ref_ptr<entity::Stroke> >& m_buffer;
    const std::vector<entity::Stroke*>& m_selected;
};

class EditPhotoPushCommand : public QUndoCommand
{
public:
    EditPhotoPushCommand(entity::UserScene* scene, entity::Canvas* current, entity::Canvas* previous,
                         entity::Photo* photo, QUndoCommand* parent=0);
    ~EditPhotoPushCommand() {}

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

protected:
    osg::observer_ptr<entity::UserScene> m_scene;
    osg::observer_ptr<entity::Canvas> m_current;
    osg::observer_ptr<entity::Canvas> m_previous;
    osg::observer_ptr<entity::Photo> m_photo;
};

#endif // EDITENTITYCOMMAND_H
