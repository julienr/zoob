#ifndef BLINE_H_
#define BLINE_H_

/* This class is only here to allow generality in grid trace methods.
 * It represents a bounding volume that is a line (ie: has no volume)
 */
class BLine: BoundingVolume {
    virtual BVolumeType getType () {
      return TYPE_LINE;
    }

    // Returns width/height of bounding box
    float getWidth () const {
      return 0;
    }

    float getHeight () const {
      return 0;
    }
};

#endif /* BLINE_H_ */
