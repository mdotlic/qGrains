#ifndef MODELNODEBASE_H
#define MODELNODEBASE_H
class QVariant;

class ModelNodeBase : public QObject
{
   Q_OBJECT
public:
   ModelNodeBase (ModelNodeBase * parent = 0);
   virtual ~ModelNodeBase();
   virtual QVariant data (int row, int col) const;
   virtual bool setData (int, int, QVariant);
   ModelNodeBase * child (int row) {return _children[row]; }
   const ModelNodeBase * child (int row) const {return _children[row]; }
   void appendChild (ModelNodeBase * child);
protected:
   std::vector<ModelNodeBase*> _children;
   std::string * _name;
private:
   ModelNodeBase * const _parent;
};

#endif
