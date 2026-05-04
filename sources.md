# Sources & References

Papers in `math/` are the local copies used during implementation.
All other entries are cited in the slides or presenter notes.
Specific factual claims (performance numbers, dates) are flagged with their source.

---

## Papers in math/ (local copies)

**Amanatides, J., & Woo, A. (1987)**
"A Fast Voxel Traversal Algorithm for Ray Tracing."
*Eurographics '87*, 3–10. University of Toronto.
→ `math/fast_voxel_traversal_algo.pdf`
*Used for:* DDA grid traversal, tMax/tDelta initialisation, rayID deduplication.
*Specific claims from this paper:* initialization costs 33 floating-point operations (inside grid) or up to 40 (outside); traversal loop costs 2 float comparisons + 1 float addition per step; intersection testing "can easily take up to 95% of the rendering time" (Introduction).

---

**Whitted, T. (1980)**
"An Improved Illumination Model for Shaded Display."
*Communications of the ACM*, 23(6), 343–349. Bell Laboratories.
→ `math/p343-whitted.pdf`
*Used for:* recursive specular reflection, refraction (Snell's law vector form), the illumination model I = I_a + k_d·ΣN·L_j + k_s·S + k_t·T.

---

**Cook, R.L., Porter, T., & Carpenter, L. (1984)**
"Distributed Ray Tracing."
*Computer Graphics (SIGGRAPH '84 Proceedings)*, 18(3), 137–145. Lucasfilm Ltd.
→ `math/distributed_ray_tracing_soft_shadows.pdf`
*Used for:* area light sampling for soft shadows; the unifying framework for distributing rays across light area, lens aperture, time (motion blur), and BRDF lobe. The shading integral I = ∬ L(φ_i,θ_i) R(φ_i,θ_i,φ_r,θ_r) dφ_i dθ_i.

---

**Blinn, J.F. (1977)**
"Models of Light Reflection for Computer Synthesized Pictures."
*SIGGRAPH '77 Proceedings*, 192–198. University of Utah.
→ `math/Blinn1977Shading.pdf`
*Used for:* the Torrance-Sparrow-based specular model S = DGF/(N̂·Ê); the Gaussian microfacet distribution D = e^(−c²α²); the V-groove geometric attenuation G; the half-vector H = (L+E)/len(L+E); and Blinn's simplification to (N̂·Ĥ)^n.

---

**Monte Carlo Path Tracing (textbook chapter)**
"Chapter 1: Monte Carlo Path Tracing."
Unpublished course notes. References Kajiya (1986) as the primary source.
Also cites: Carter, L. & Cashwell, E., *Particle-Transport Simulation with the Monte Carlo Methods*; Spanier, J. & Gelbard, E., *Monte Carlo Principles and Neutron Transport Problems*.
→ `math/monte_carlo_path_tracing.pdf`
*Used for:* the path tracer implementation structure (throughput, Russian Roulette, NEE).

---

## Core Algorithm Papers (cited in slides)

**Lambert, J.H. (1760)**
*Photometria*. Augsburg: Eberhardt Klett.
*Used for:* Lambert's cosine law (irradiance E = Φ cos θ); the Lambertian BRDF f_r = ρ/π.

---

**Torrance, K.E., & Sparrow, E.M. (1967)**
"Theory for Off-Specular Reflection from Roughened Surfaces."
*Journal of the Optical Society of America*, 57(9), 1105–1114.
*Used for:* the microfacet model underlying Blinn-Phong (cited in the Blinn 1977 paper).

---

**Appel, A. (1968)**
"Some Techniques for Shading Machine Renderings of Solids."
*Proceedings of the AFIPS Spring Joint Computer Conference*, 32, 37–45.
*Used for:* the ray casting baseline E(D|G)L in the path notation table (Slide 15).

---

**Goral, C.M., Torrance, K.E., Greenberg, D.P., & Battaile, B. (1984)**
"Modeling the Interaction of Light between Diffuse Surfaces."
*SIGGRAPH '84 Computer Graphics*, 18(3), 213–222.
*Used for:* radiosity (ED\*L path coverage); the Cornell Box scene as a validation test.

---

**Kajiya, J.T. (1986)**
"The Rendering Equation."
*SIGGRAPH '86 Computer Graphics*, 20(4), 143–150.
*Used for:* the rendering equation L_o = L_e + ∫ f_r L_i cos θ dω; the operator form L = L_e + K∘L; the Neumann series solution; the concept of path tracing as Monte Carlo sampling of the series. Also the original source for the path notation idea (formalised later by Shirley).

---

**Marsaglia, G. (1972)**
"Choosing a Point from the Surface of a Sphere."
*The Annals of Mathematical Statistics*, 43(2), 645–646.
*Used for:* uniform sphere sampling via rejection from the unit disk (soft shadows, Slide 13). Expected rejection rate 1 − π/4 ≈ 21.5%.

---

**Schlick, C. (1994)**
"An Inexpensive BRDF Model for Physically-Based Rendering."
*Computer Graphics Forum (Eurographics '94)*, 13(3), 233–246.
*Used for:* the Schlick Fresnel approximation F ≈ F₀ + (1−F₀)(1−cosθ)⁵ mentioned in presenter notes for Slide 9.

---

**Lafortune, E.P., & Willems, Y.D. (1993)**
"Bi-Directional Path Tracing."
*Proceedings of Compugraphics '93*, 145–153.
*Used for:* bidirectional path tracing reference in Slides 31 and 35.

---

**Jensen, H.W. (1996)**
"Global Illumination using Photon Maps."
*Rendering Techniques '96 (Eurographics Workshop on Rendering)*, 21–30.
*Used for:* photon mapping as an alternative caustic solution (Slide 33 open problems).

---

**Veach, E. (1997)**
"Robust Monte Carlo Methods for Light Transport Simulation."
PhD dissertation, Stanford University.
*Used for:* Multiple Importance Sampling (MIS) and the power heuristic w_i = p_i^k / Σ p_j^k; proof that k=2 is optimal in a minimax sense. Also the definitive reference for bidirectional path tracing MIS weights (Slide 35).

---

**Veach, E., & Guibas, L.J. (1997)**
"Metropolis Light Transport."
*SIGGRAPH '97 Computer Graphics Proceedings*, 65–76.
*Used for:* Metropolis Light Transport (MLT) reference in Slide 35.

---

**Walter, B., Marschner, S.R., Li, H., & Torrance, K.E. (2007)**
"Microfacet Models for Refraction through Rough Surfaces."
*Rendering Techniques 2007 (Eurographics Symposium on Rendering)*, 195–206.
*Used for:* GGX microfacet distribution; state-of-the-art replacement for Blinn-Phong (Slides 11, 34, 35).

---

**Heitz, E. (2014)**
"Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs."
*Journal of Computer Graphics Techniques (JCGT)*, 3(2), 32–91.
*Used for:* VNDF (Visible Normal Distribution Function) importance sampling for GGX, referenced in Slide 34 next steps.

---

**Wilkie, A., Nawaz, S., Droske, M., Weidlich, A., & Hanika, J. (2014)**
"Hero Wavelength Spectral Sampling."
*Computer Graphics Forum (EGSR '14)*, 33(4), 123–131.
*Used for:* spectral rendering reference in Slide 35.

---

**Bitterli, B., Wyman, C., Pharr, M., Shirley, P., Lefohn, A., & Jarosz, W. (2020)**
"Spatiotemporal Reservoir Resampling for Real-Time Ray Tracing with Dynamic Direct Lighting."
*ACM Transactions on Graphics (SIGGRAPH '20)*, 39(4), Article 148.
*Used for:* ReSTIR (Slide 29); weighted reservoir sampling; spatial and temporal reuse of light samples. The claim "quality equivalent to sampling hundreds of lights at cost of 1–2 shadow rays per pixel" comes from this paper's results section.

---

**Ouyang, Y., Liu, S., Kettunen, M., Pharr, M., & Panteleev, J. (2021)**
"ReSTIR GI: Path Resampling for Real-Time Path Tracing."
*High-Performance Graphics 2021*.
*Used for:* ReSTIR GI extension to indirect illumination (Slides 14, 29).

---

**Müller, T., Rousselle, F., Novák, J., & Keller, A. (2021)**
"Real-Time Neural Radiance Caching for Path Tracing."
*ACM Transactions on Graphics (SIGGRAPH '21)*, 40(4), Article 36. NVIDIA.
*Used for:* Neural Radiance Cache (NRC) reference in Slides 33 and 35.

---

## Production Renderers

**Christensen, P., Fong, J., Shade, J., Wooten, W., Schubert, B., Kensler, A., ... & Hery, C. (2018)**
"RenderMan: An Advanced Path-Tracing Architecture for Movie Rendering."
*ACM Transactions on Graphics*, 37(3), Article 30. Pixar Animation Studios.
*Used for:* Pixar RenderMan switching to path tracing; MIS implementation; next-vertex estimation; manifold exploration for caustics (Slide 31).
*Specific claim:* Finding Dory (2016) was the first Pixar feature rendered with the path-traced RenderMan pipeline — from this paper and associated Pixar SIGGRAPH presentations.

---

**Burley, B., Adler, D., Chiang, M.J.Y., Driskill, H., Habel, R., Kelly, P., ... & Meng, J. (2018)**
"The Design and Evolution of Disney's Hyperion Renderer."
*ACM Transactions on Graphics*, 37(3), Article 33. Walt Disney Animation Studios.
*Used for:* Disney Hyperion; bidirectional path tracing for caustics and difficult indirect paths (Slide 31).

---

**Karis, B. (2013)**
"Real Shading in Unreal Engine 4."
*SIGGRAPH 2013 Course: Physically Based Shading in Theory and Practice*. Epic Games.
*Used for:* the split-sum PBR approximation combining Lambert diffuse + GGX specular; the observation that Lambert remains the diffuse standard in PBR pipelines (Slides 8, 11).

---

**Lagarde, S., & de Rousiers, C. (2014)**
"Moving Frostbite to Physically Based Rendering 3.0."
*SIGGRAPH 2014 Course: Physically Based Shading in Theory and Practice*. EA DICE.
*Used for:* industry transition from Blinn-Phong to GGX PBR ~2012–2014 (Slide 11).

---

**Epic Games. (2022)**
"Lumen Global Illumination and Reflections."
*Unreal Engine 5 Documentation and SIGGRAPH 2022 presentation.*
*Used for:* screen-space tracing, radiance cache, surface cache, two-level tracing (Slide 32).

---

## Hardware & Industry

**NVIDIA Corporation. (2018)**
"NVIDIA Turing GPU Architecture." NVIDIA Whitepaper.
*Used for:* RT Cores for BVH traversal and ray-triangle intersection; two-level BVH (BLAS/TLAS); DXR/Vulkan RT API; RTX hardware launch in 2018 (Slide 28).

---

**NVIDIA Corporation. (2022)**
"GeForce RTX 4090 Product Page and Ada Lovelace Architecture Whitepaper."
*Used for:* the **~10 Giga-rays/sec** figure on Slide 28.
⚠ *Note:* This figure is an approximation from NVIDIA's RT performance benchmarks for the Ada Lovelace architecture. Actual throughput varies significantly with scene complexity, BVH quality, and shader workload. The **~100M rays/sec** figure for CPU software is a typical order-of-magnitude estimate for a single-threaded path tracer on a modern CPU core, not from a specific benchmark.

---

**NVIDIA Corporation. (2017)**
"NVIDIA OptiX AI-Accelerated Denoiser."
NVIDIA Developer Blog and OptiX SDK Documentation.
*Used for:* CNN-based Monte Carlo denoiser taking noisy colour + G-buffer → clean image (Slide 30).

---

**NVIDIA Corporation. (2023)**
"DLSS 3.5 Ray Reconstruction."
NVIDIA Press Release and Technical Blog, August 2023.
*Used for:* joint denoising + temporal upscaling replacing running average; "rendered at 1080p with 1 spp → displayed at 4K" description (Slide 30).
⚠ *Note:* The specific "1080p/1spp → perceived quality of many spp at 4K" claim is from NVIDIA marketing material, not a peer-reviewed benchmark.

---

**CD Projekt Red & NVIDIA. (2023)**
"Cyberpunk 2077 Overdrive Mode Path Tracing."
NVIDIA blog post and CD Projekt Red developer presentations, April 2023.
*Used for:* Cyberpunk 2077 as a production deployment of ReSTIR (Slide 29).

---

**Remedy Entertainment & NVIDIA. (2023)**
"Alan Wake 2: Full Path Tracing."
Remedy Entertainment developer presentations and NVIDIA blog, October 2023.
*Used for:* Alan Wake 2 as a production deployment of full path tracing with RT hardware (Slides 18, 29).

---
