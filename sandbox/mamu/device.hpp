#ifndef GOTT_GUI_DEVICE_HPP_INCLUDED
#define GOTT_GUI_DEVICE_HPP_INCLUDED


namespace gott{namespace gui{
  
struct device_property {
  enum PixelLayout {
    CRTDiffuse
      , LCDVertical
      , LCDHorizontal
  };
  PixelLayout dot_layout;
  size_t v_dpi
    , h_dpi;
};

}}

#endif

