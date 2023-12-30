#include "canvas2d.h"
#include <QPainter>
#include <QMessageBox>
#include <QFileDialog>
#include <iostream>
#include "settings.h"

/**
 * @brief Initializes new 500x500 canvas
 */
void Canvas2D::init() {
    setMouseTracking(true);
    m_width = 500;
    m_height = 500;
    clearCanvas();
}

/**
 * @brief Canvas2D::clearCanvas sets all canvas pixels to blank white
 */
void Canvas2D::clearCanvas() {
    m_data.assign(m_width * m_height, RGBA{255, 255, 255, 255});
    settings.imagePath = "";
    displayImage();
}

/**
 * @brief Stores the image specified from the input file in this class's
 * `std::vector<RGBA> m_image`.
 * Also saves the image width and height to canvas width and height respectively.
 * @param file: file path to an image
 * @return True if successfully loads image, False otherwise.
 */
bool Canvas2D::loadImageFromFile(const QString &file) {
    QImage myImage;
    if (!myImage.load(file)) {
        std::cout<<"Failed to load in image"<<std::endl;
        return false;
    }
    myImage = myImage.convertToFormat(QImage::Format_RGBX8888);
    m_width = myImage.width();
    m_height = myImage.height();
    QByteArray arr = QByteArray::fromRawData((const char*) myImage.bits(), myImage.sizeInBytes());

    m_data.clear();
    m_data.reserve(m_width * m_height);
    for (int i = 0; i < arr.size() / 4.f; i++){
        m_data.push_back(RGBA{(std::uint8_t) arr[4*i], (std::uint8_t) arr[4*i+1], (std::uint8_t) arr[4*i+2], (std::uint8_t) arr[4*i+3]});
    }
    displayImage();
    return true;
}

/**
 * @brief Saves the current canvas image to the specified file path.
 * @param file: file path to save image to
 * @return True if successfully saves image, False otherwise.
 */
bool Canvas2D::saveImageToFile(const QString &file) {
    QImage myImage = QImage(m_width, m_height, QImage::Format_RGBX8888);
    for (int i = 0; i < m_data.size(); i++){
        myImage.setPixelColor(i % m_width, i / m_width, QColor(m_data[i].r, m_data[i].g, m_data[i].b, m_data[i].a));
    }
    if (!myImage.save(file)) {
        std::cout<<"Failed to save image"<<std::endl;
        return false;
    }
    return true;
}


/**
 * @brief Get Canvas2D's image data and display this to the GUI
 */
void Canvas2D::displayImage() {
    QByteArray* img = new QByteArray(reinterpret_cast<const char*>(m_data.data()), 4*m_data.size());
    QImage now = QImage((const uchar*)img->data(), m_width, m_height, QImage::Format_RGBX8888);
    setPixmap(QPixmap::fromImage(now));
    setFixedSize(m_width, m_height);
    update();
}

/**
 * @brief Canvas2D::resize resizes canvas to new width and height
 * @param w
 * @param h
 */
void Canvas2D::resize(int w, int h) {
    m_width = w;
    m_height = h;
    m_data.resize(w * h);
    displayImage();
}

/**
 * @brief Called when the filter button is pressed in the UI
 */
void Canvas2D::filterImage() {
    // Filter TODO: apply the currently selected filter to the loaded image

}

/**
 * @brief Called when any of the parameters in the UI are modified.
 */
void Canvas2D::settingsChanged() {
    // this saves your UI settings locally to load next time you run the program
    settings.saveSettings();

    // TODO: fill in what you need to do when brush or filter parameters change
}

bool Canvas2D::isInBounds(int x, int y) {
    return x>=0&&x<m_width&&y>=0&&y<m_height;
}

int Canvas2D::posToCanvasIndex(int x, int y, int width) {
    int index = y * m_width + x;
    return index;
}

bool Canvas2D::isInMask(int x, int y, int mx, int my) {
    return pixelDistance(x,y,mx,my)>=settings.brushRadius;
}

float Canvas2D::pixelDistance(int x1, int y1, int x2, int y2) {
    return sqrt((float)(x1-x2)*(x1-x2)+(float)(y1-y2)*(y1-y2));
}

RGBA Canvas2D::blendPixels(RGBA brushPixel,RGBA canvasPixel,float opacity) {
    RGBA res;
    res.r = opacity * brushPixel.a/255.0 * brushPixel.r/255.0 + (1.0 - (opacity*brushPixel.a/255.0)) * canvasPixel.r/255.0;
    res.g = opacity * brushPixel.a/255.0 * brushPixel.g/255.0 + (1.0 - (opacity*brushPixel.a/255.0)) * canvasPixel.g/255.0;
    res.b = opacity * brushPixel.a/255.0 * brushPixel.b/255.0 + (1.0 - (opacity*brushPixel.a/255.0)) * canvasPixel.b/255.0;
    res.a = brushPixel.a/255.0 + canvasPixel.a/255.0*(1.0-brushPixel.a/255.0);
    return res;
}

std::vector<RGBA> Canvas2D::brushConstantMask(int x, int y) {
    int canvasIndex = posToCanvasIndex(x,y,m_width);
    std::vector<RGBA> res;
    RGBA blank={0,0,0,0};

    for(int ix=0;ix<2*settings.brushRadius+1;ix++) {
        for(int iy=0;iy<=2*settings.brushRadius+1;iy++) {
            if(isInMask(ix,iy,settings.brushRadius,settings.brushRadius)){
                res.push_back(settings.brushColor);
            }
            else {
                res.push_back(blank);
            }
        }
    }
    return res;
}

void Canvas2D::applyMask(std::vector<RGBA> mask, int x, int y) {
    for(int i=0;i<mask.size();i++) {
        int ix = x + (i / (2*settings.brushRadius + 1))-settings.brushRadius;
        int iy = y + (i % (2*settings.brushRadius + 1))-settings.brushRadius;
        m_data[posToCanvasIndex(ix,iy,m_width)] = blendPixels(settings.brushColor,m_data[posToCanvasIndex(ix,iy,m_width)],1.0);
    }
}


/**
 * @brief These functions are called when the mouse is clicked and dragged on the canvas
 */
void Canvas2D::mouseDown(int x, int y) {
    m_isDown = true;
    applyMask(brushConstantMask(x,y),x,y);
    displayImage();
}

void Canvas2D::mouseDragged(int x, int y) {
    // Brush TODO
}

void Canvas2D::mouseUp(int x, int y) {
    // Brush TODO
}