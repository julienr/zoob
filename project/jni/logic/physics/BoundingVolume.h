#ifndef BOUNDINGVOLUME_H_
#define BOUNDINGVOLUME_H_

enum BVolumeType {
  TYPE_AABBOX = 0,
  TYPE_CIRCLE,
  TYPE_LINE,
};

struct BoundingVolume {
  virtual ~BoundingVolume() {}
  virtual BVolumeType getType () const = 0;

  // Returns width/height of bounding box
  virtual float getWidth () const = 0;
  virtual float getHeight () const = 0;
};

#endif /* BOUNDINGVOLUME_H_ */
