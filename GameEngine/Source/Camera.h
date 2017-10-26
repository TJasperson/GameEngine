//
//  Camera.h
//  GameEngine
//
//  Created by Tim Arterbury on 10/18/17.
//
//

#pragma once

#include "glm/glm.hpp"

/** Represents a simple 2D camera that can be positioned.
    
    Future WishList:
        - Rotations
        - Full 3D usage
 */
class Camera
{
public:
    
    /** Initializes the view matrix
     */
    Camera()
    {
        // Initialize to identity
        viewMatrix = glm::mat4(1.0);
        
        // Set the Z position of camera to be at 0 since this is a flat scene
        viewMatrix[3][2] = 0.0f;
        
        // Set default scale
        setScale(1.0f);
    }
    
    /** Gets the view matrix of the Camera
     */
    glm::mat4 getViewMatrix()
    {
        return viewMatrix;
    }
    
    /** Sets xyz position of the camera. Since this camera is only really 2D
        right now, we don't need a a look vector.
     */
    void setPosition(glm::vec3 position)
    {
        viewMatrix[3][0] = position.x;
        viewMatrix[3][1] = position.x;
        viewMatrix[3][1] = position.z;
    }
    
    /** Sets x position of the camera. For a side scroller, we will use this to
        set the position of the camera based on the player's movement.
     */
    void setXPosition(float x)
    {
        viewMatrix[3][0] = x;
    }

    
    /** Sets the scale or zoom of the camera.
     */
    void setScale (float scale)
    {
        viewMatrix[0][0] = scale;
        viewMatrix[1][1] = scale;
        viewMatrix[2][2] = scale;
    }
    
private:
    glm::mat4 viewMatrix;
};