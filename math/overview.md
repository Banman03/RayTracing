# In Nature
- A light source emits a ray of light (stream of photons) that travel along a path until they are interrupted by an object.
- For our purposes, this ray will be a straight line (we are not simulating a black hole at this point).
- We will ignore absorbtion, refraction, and flourescence, instead assuming that all light that hits an object is reflected back into the camera.

# Mathematical Overview of Algorithm
- More accurate (in terms of "realness" of image) than raycasting or rasterization.
- Works by tracing a pixel from an imaginary eye (camera) through each pixel in a scene and calculating the color of the object through that pixel.
- All rays in the view of the camera must be tested for intersection with a subset of the objects on the screen. I have a feeling we can be smart about this.
    - We can likely exclude objects from collision detection based upon their center (we will start with spheres) and their radii, compared to the direction/origin of a given ray.
    - Additionally, rays will not be passing through targets, so we only need to consider the first object a ray hits (objects are completely occluded right now).
- Upon collision, we take into consideration the properties of the object that the light hit and the amount of light hitting the object, and thus determine a pixel color.
    - For now, rays do not bounce around, so we only need to consider objects within the FOV of the camera.

## Inputs
- $E$ (camera position)
- $T$ (target position)
- $\theta$ (FOV; assume 90 degrees)
- $m$, $k$ (number of square pixels in the vertical and horizontal direction)
- $i$, $j$ (indexing for square number; i indexes horizontally, j indexes vertically)
- $\vec{v}$ (determines the up/down direction. typically [0, 1, 0])

## Process 
- For each pixel in the scene, we want to:
    ### Precalculations
        - Determine the normalized $\vec{b}$, $\vec{t}$, $\vec{v}$ so that we can determine the coordinates of $P_{i, j}$ relative to $P_{1, j}$. This is essential so that we can know our vectors which are parallel to the viewport.
        - $\vec{t} = T - E$
        - $vec{b} = \vec{t} \times \vec{v}$
        - $\vec{t_n} = norm(\vec{t})$ (same for $\vec{b}$).
        - $\vec{v_n} = \vec{t_n} \times \vec{b_n}$
        - $g_x = d*\tan{\theta}/2$ assume $d = 1$.
        - $g_y = g_x * \frac{m - 1}{k - 1}$.
        - We then need to calculate the vectors denoting shift in pixel position along the v or b axis:
        - $\vec{q_x} = \frac{2g_x}{k - 1} \vec{b_n}$.
        - $\vec{q_y} = \frac{2g_y}{m - 1} \vec{v_n}$.
        - $\vec{p_{1, m}} = \vec{t_n}d - g_x\vec{b_n} - g_y\vec{v_n}$

    - Determine $r_{, j} = P_{i, j} - E$. ($R$ is the unnormalized version):
    - $\vec{p_{i, j}} = \vec{p_{1, m}} + \vec{q_x}(i - 1) + \vec{q_y}(j - 1)$
    - $\vec{r_{i, j}} = \frac{\vec{p_{i, j}}}{\norm{r_{i, j}}}$