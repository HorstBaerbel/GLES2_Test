#pragma once

#include <string>
#include <vector>

#include "../math/Math.h"

/*!
.obj file loader/saver


.obj file format (excerpt from here http://people.sc.fsu.edu/~jburkardt/txt/obj_format.txt):
---------------------------------------------------------------------------------------------
Comments: "# ...\n"

Vertex: "v <X> <Y> <Z>\n"

Normal: "vn <X> <Y> <Z>\n"

Texture coordinate: "vt <U> [<V>] [<W>]\n"

Point: "p <Index vertex 1> <Index vertex 2> ...\n" (vertices)
       Indices can be negative and then indicate a relative offset to the current end of the vertex array

Line: "l <Index vertex 1> <Index vertex 2> ...\n" (vertices) or
      "l <Index vertex 1>/<Index texcoord 1> <Index vertex 2>/<Index texcoord 2> ...\n" (vertices/textures)
      Indices can be negative and then indicate a relative offset to the current end of the vertex array

Face: "f <Index vertex 1> <Index vertex 2> ...\n" (vertices) or
      "f <Index vertex 1>/<Index texcoord 1> <Index vertex 2>/<Index texcoord 2> ...\n" (vertices/textures) or
      "f <Index vertex 1>//<Index normal 1> <Index vertex 2>//<Index normal 2> ...\n" (vertices//normals) or
      "f <Index vertex 1>/<Index texcoord 1>/<Index normal 1> <Index vertex 2>/<Index texcoord 2>/<Index normal 2> ...\n" (vertices/textures/normals)
      Indices can be negative and then indicate a relative offset to the current end of the vertex array

Group: "g <GROUP_NAME>\n"

Object: "o <OBJECT_NAME>\n"

Material file: "mtllib <MTL_FILE_NAME>\n"

Material: "usemtl <MATERIAL_NAME>\n" (null) specifies a default material


.mtl file format (excerpt from here http://people.sc.fsu.edu/~jburkardt/data/mtl/mtl.html):
--------------------------------------------------------------------------------------------
Ambient: "Ka r g b" defines the ambient color of the material to be (r,g,b).
         The default is (0.2,0.2,0.2); 

Diffuse: "Kd r g b" defines the diffuse color of the material to be (r,g,b).
         The default is (0.8,0.8,0.8); 

Specular: "Ks r g b" defines the specular color of the material to be (r,g,b). This color shows up in highlights.
          The default is (1.0,1.0,1.0); 

Alpha: "d alpha" or "Tr alpha" defines the transparency of the material to be alpha.
	   The default is 1.0 (not transparent at all).

Shininess: "Ns s" defines the shininess of the material to be s.
           The default is 0.0; 

Model: "illum n" denotes the illumination model used by the material. 
        illum = 1 indicates a flat material with no specular highlights, so the value of Ks is not used.
		illum = 2 denotes the presence of specular highlights, and so a specification for Ks is required. 

Texture: "map_Ka filename" names a file containing a ambient texture map
         "map_Kd filename" names a file containing a diffuse texture map
         "map_Ks filename" names a file containing a specular texture map
*/

class ModelObj
{
public:
    enum PrimitiveType {POINTS = 0, LINES = 1, LINE_STRIP = 3, TRIANGLES = 4, QUADS = 7, POLYGON = 9}; //!<These correspond to the GL_ types

    struct Mesh {
        int * vertexIndices;
        int * normalIndices;
        int * tangentIndices;
        int * textureIndices;
        int nrOfIndices;
        PrimitiveType primitiveType;
        int nrOfPrimitives;
        Material material;

        Mesh();
    };

    struct Model {
        std::string name;
        vec3 * vertices;
        int nrOfVertices;
        vec3 * normals;
        int nrOfNormals;
        vec3 * tangents;
        int nrOfTangents;
        void * texcoords;
        int nrOfTexCoords;
        int texCoordSize; //!<Either 0(if none)/1/2/3
        std::vector<Mesh> meshes;

        Model();
    };

    static Material DefaultMaterial;

private:
    std::vector<Material> materials;
    std::vector<Model> models;
    std::string creator;

    void freeModel(Model & model);
    void freeAll();

public:
    ObjModel();
    
    bool fromFile(const std::string & fileName);
    bool fromString(const std::string & data);
    bool fromRaw(const char * data, const int size);

    bool materialsFromFile(const std::string & fileName);
    bool materialsFromString(const std::string & data);
    bool materialsFromRaw(const char * data, const int size);

    bool toFile(const std::string & fileName);
    bool toString(const std::string & data);

    /*!
    Save to raw data. Data is allocate for you! YOU NEED TO DELETE IT!
    \param[in] data Reference to data destination pointer. Remeber to delete it after usage.
    \param[out] size. Reference to size of returned data.
    \return Returns true if all went well.
    */
    bool toRaw(char * & data, int & size);

    int getNrOfModels() const;
    const Model & getModel(unsigned int nr) const;

    ~ObjModel(void);
};

