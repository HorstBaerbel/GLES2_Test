#include "ModelObj.h"

#include <iostream>
#include <fstream>
#include <sstream>


ModelObj::Material ModelObj::DefaultMaterial;

ModelObj::Material::Material()
{
    ambient.r = 0.2f;
    ambient.g = 0.2f;
    ambient.b = 0.2f;
    diffuse.r = 0.8f;
    diffuse.g = 0.8f;
    diffuse.b = 0.8f;
    specular.r = 1.0f;
    specular.g = 1.0f;
    specular.b = 1.0f;
    alpha = 1.0f;
    shininess = 0.0f;
    model = 1;
}

ModelObj::Mesh::Mesh()
    : vertexIndices(NULL), normalIndices(NULL), tangentIndices(NULL), textureIndices(NULL), nrOfIndices(0),
    primitiveType((PrimitiveType)-1), nrOfPrimitives(0), material(DefaultMaterial)
{
}

ModelObj::Model::Model()
    : vertices(NULL), nrOfVertices(0),
    normals(NULL), nrOfNormals(0),
    tangents(NULL), nrOfTangents(0),
    texcoords(NULL), nrOfTexCoords(0), texCoordSize(0)
{
}


ModelObj::ModelObj(void)
{
}

//----- load ------------------------------------------------------------------------------------------------------------

bool ModelObj::fromFile(const std::string & fileName)
{
    bool result = false;

    std::ifstream file;
    file.open(fileName, std::ios_base::in | std::ios_base::binary);
    //retrieve length of file. we fail to support ascii files > 2GB here. sorry.
    file.seekg(0, std::ios::end);
    const int length = file.tellg();
    file.seekg(0, std::ios::beg);
    if (0 < length) {
        //if proper length, allocate memory and read file content
        char * data = new char[length];
        file.read(data, length);
        //load file from stream
        result = fromRaw(data, length);
        //free data
        delete [] data;
    }
    //close file
    file.close();
    return false;
}

bool ModelObj::fromRaw(const char * data, const int size)
{
    if (NULL != data && 0 < size) {
        return fromString(std::string(data, size));
    }
    return false;
}

int countElements(const std::string in)
{
    int count = 0;
    size_t lastPos = 0;
    while (std::string::npos > lastPos) {
        lastPos = in.find(' ', lastPos + 1);
        count++;
    }
    return count;
}

ModelObj::PrimitiveType primitiveType(const std::string command, const std::string in, const int currentLine)
{
    const int count = countElements(in);
    ModelObj::PrimitiveType result = (ModelObj::PrimitiveType)-1;
    if ("p" == command) {
        if (0 >= count) {
            printf("Too few (%d) indices for type point (Line %d).\n", count, currentLine);
        }
        else {
            result = ModelObj::POINTS;
        }
    }
    else if ("l" == command) {
        if (2 > count) {
            printf("Too few (%d) indices for type line (Line %d).\n", count, currentLine);
        }
        else if (2 == count) {
            result = ModelObj::LINES;
        }
        else {
            result = ModelObj::LINE_STRIP;
        }
    }
    else if ("f" == command) {
        if (3 > count) {
            printf("Too few (%d) indices for type face (Line %d).\n", count, currentLine);
        }
        else if (3 == count) {
            result = ModelObj::TRIANGLES;
        }
        else if (4 == count) {
            result = ModelObj::QUADS;
        }
        else {
            result = ModelObj::POLYGON;
        }
    }
    return result;
}

std::vector<ModelObj::Mesh>::iterator meshCommandAllocate(std::vector<ModelObj::Model>::iterator modelIter, std::vector<ModelObj::Mesh>::iterator meshIter, const std::string command, const std::string content, const int currentLine)
{
    //count elements in content
    const int count = countElements(content);
    //check what type the face has
    ModelObj::PrimitiveType foundType = primitiveType(command, content, currentLine);
    //compare to type of current mesh
    if (-1 >= meshIter->primitiveType) {
        //if the current mesh has a bad primitive type this means it is uninitialized. set it
        meshIter->primitiveType = foundType;
    }
    else if (foundType != meshIter->primitiveType || foundType == ModelObj::LINE_STRIP || foundType == ModelObj::POLYGON) {
        //the mesh already has a mesh type, but it isn't the same, start a new point mesh
        //if it is LINE_STRIP or POLYGON we need to start a new mesh anyway, 
        //because otherwise the line strips or polygons would be connected...
        ModelObj::Mesh newMesh;
        newMesh.primitiveType = foundType;
        modelIter->meshes.push_back(newMesh);
        meshIter = modelIter->meshes.begin() + modelIter->meshes.size() - 1;
    }
    meshIter->nrOfIndices += count;
    meshIter->nrOfPrimitives++;
    return meshIter;
}

std::vector<ModelObj::Mesh>::iterator meshCommandRead(std::vector<ModelObj::Model>::iterator modelIter, std::vector<ModelObj::Mesh>::iterator meshIter, const std::string command, const std::string content, const int currentLine)
{
    //count elements in content
    const int count = countElements(content);
    //check what type the face has
    ModelObj::PrimitiveType foundType = primitiveType(command, content, currentLine);
    //compare to type of current mesh
    if (foundType != meshIter->primitiveType || foundType == ModelObj::LINE_STRIP || foundType == ModelObj::POLYGON) {
        //the mesh already has a mesh type, but it isn't the same, switch to next mesh
        //if it is LINE_STRIP or POLYGON we need to switch to a new mesh anyway, 
        //because otherwise the line strips or polygons would be connected...
        meshIter++;
    }
    //read data from command
    //create stream object from string
    std::istringstream cstream(content);
    while (!cstream.eof()) {
        //retrieve first element
        std::string element;
        cstream >> element;
        //count how many parts (vertex/texcoord/normal) this has. all following elements have to have the same
        //no '/'   --> only vertex index
        //one '/'  --> vertex and texture index
        //one "//" --> vertex and normal index
        //two '/'  --> vertex, texture and normal index
        bool hasNormals = false;
        bool hasTextures = false;
        if (element.find("//") != std::string::npos) {
            hasNormals = true;
        }
        else if (element.find("/") != std::string::npos) {
            hasTextures = true;
            if (element.find("/") != element.rfind("/")) {
                hasNormals = true;
            }
        }
        //now we can allocate the index array as now we know which elements actually exist
        bool wasAllocation = false;
        if (NULL == meshIter->vertexIndices) {
            meshIter->vertexIndices = new int[meshIter->nrOfIndices];
            wasAllocation = true;
        }
        if (hasNormals) {
            if (NULL == meshIter->normalIndices) {
                meshIter->normalIndices = new int[meshIter->nrOfIndices];
                wasAllocation = true;
            }
        }
        if (hasTextures) {
            if (NULL == meshIter->textureIndices) {
                meshIter->textureIndices = new int[meshIter->nrOfIndices];
                wasAllocation = true;
            }
        }
        if (wasAllocation) {
            //this is the first element of the mesh, clear counters
            meshIter->nrOfIndices = 0;
            meshIter->nrOfPrimitives = 0;
        }
        //now get elements from stream
        cstream.clear();
        cstream.seekg(0, std::ios::beg);
        while (!cstream.eof()) {
            int value = 0;
            cstream >> element;
            std::istringstream estream(element);
            //get vertex index
            estream >> value;
            meshIter->vertexIndices[meshIter->nrOfIndices] = value - 1;
            //if this has a texture coordinate read it
            if (hasTextures) {
                //discard '/'
                estream.ignore(1, '/');
                //read texture coord
                estream >> value;
                meshIter->textureIndices[meshIter->nrOfIndices] = value - 1;
                if (hasNormals) {
                    //discard '/'
                    estream.ignore(1, '/');
                    //read normal
                    estream >> value;
                    meshIter->normalIndices[meshIter->nrOfIndices] = value - 1;
                }
            }
            else {
                if (hasNormals) {
                    //discard '//'
                    estream.ignore(1, '/');
                    estream.ignore(1, '/');
                    //read normal
                    estream >> value;
                    meshIter->normalIndices[meshIter->nrOfIndices] = value - 1;
                }
            }
            meshIter->nrOfIndices++;
        }
    }
    meshIter->nrOfPrimitives++;
    return meshIter;
}

bool ModelObj::fromString(const std::string & data)
{
    //clear previous models
    freeAll();
    //create stream object from string
    std::istringstream stream(data);
    //create initial model
    Model _model;
    models.push_back(_model);
    std::vector<Model>::iterator modelIter = models.begin();
    //create initial mesh in model
    Mesh _mesh;
    modelIter->meshes.push_back(_mesh);
    std::vector<Mesh>::iterator meshIter = modelIter->meshes.begin();
    //create initial material
    Material _material;
    materials.push_back(_material);
    std::vector<Material>::iterator materialIter = materials.begin();
    int currentLine = 0;
    //---------------------------------- first run through data ------------------------------------------
    //do first run through data, count everything and build structures
    while (!stream.eof()) {
        currentLine++;
        std::string command;
        std::string content;
        //read line from stream
        std::getline(stream, command);
        //command smaller than 3 characters do not exist
        if (command.length() < 3) {
            continue;
        }
        //separate line into command and content
        size_t spacePos = command.find(' ');
        content = command.substr(spacePos + 1);
        command = command.substr(0, spacePos);
        //trim whitespace at the end of content
        content = content.erase(content.find_last_not_of(" \n\r\t") + 1);
        //check what command was found
        if ("#" == command) { //comment. only first comment is used as creator string, all others are discarded
            if (creator.empty()) {
                creator = content;
            }
        }
        else if ("v" == command) { //vertex
            modelIter->nrOfVertices++;
        }
        else if ("vt" == command) { //texture coordinate u [v] [w]
            if (0 >= modelIter->texCoordSize) {
                //check what type the texture coordinates have
                const int count = countElements(content);
                if (count >= 1 && count <= 3) {
                    modelIter->texCoordSize = count;
                }
                else {
                    printf("Bad texture coordinates size %d!\n", count);
                }
            }
            modelIter->nrOfTexCoords++;
        }
        else if ("vn" == command) { //normal
            modelIter->nrOfNormals++;
        }
        else if ("p" == command) { //point
            meshIter = meshCommandAllocate(modelIter, meshIter, command, content, currentLine);
        }
        else if ("l" == command) { //line
            meshIter = meshCommandAllocate(modelIter, meshIter, command, content, currentLine);;
        }
        else if ("f" == command) { //face
            meshIter = meshCommandAllocate(modelIter, meshIter, command, content, currentLine);
        }
        else if ("g" == command) { //group statement
            //ignore
        }
        else if ("o" == command) { //new model
            //if the current model has no name, set name, else create new model
            if (modelIter->name.empty()) {
                modelIter->name = content;
            }
            else {
                //create new model
                Model newModel;
                newModel.name = content;
                models.push_back(newModel);
                modelIter++;
                //create initial mesh
                Mesh newMesh;
                modelIter->meshes.push_back(newMesh);
                meshIter = modelIter->meshes.begin();
            }
        }
        else if ("mtllib" == command) { //material library
            //load material library
        }
        else if ("usemtl" == command) { //use material with name
            //check if material with name exits
            //add to model
        }
        else {
            //unknown tag
        }
    } //while(!stream.eof())
    //---------------------------------- second run through data ------------------------------------------
    //initial model is first one
    modelIter = models.begin();
    meshIter = modelIter->meshes.begin();
    materialIter = materials.begin();
    //seek to start again
    currentLine = 0;
    stream.clear();
    stream.seekg(0, std::ios::beg);
    //do second run through data, read actual data
    while (!stream.eof()) {
        currentLine++;
        std::string command;
        std::string content;
        //read line from stream
        std::getline(stream, command);
        //command smaller than 3 characters do not exist
        if (command.length() < 3) {
            continue;
        }
        //separate line into command and content
        size_t spacePos = command.find(' ');
        content = command.substr(spacePos + 1);
        command = command.substr(0, spacePos);
        //trim whitespace at the end of content
        content = content.erase(content.find_last_not_of(" \n\r\t") + 1);
        //check what command was found
        if ("v" == command) { //vertex
            //check if array needs to be allocated
            if (NULL == modelIter->vertices) {
                modelIter->vertices = new Vector3f[modelIter->nrOfVertices];
                modelIter->nrOfVertices = 0;
            }
            //create stream object from string
            std::istringstream cstream(content);
            Vector3f v;
            //get vertex data from stream and copy to vertex array
            cstream >> v.x >> v.y >> v.z;
            modelIter->vertices[modelIter->nrOfVertices++] = v;
        }
        else if ("vt" == command) { //texture coordinate u [v] [w]
            //check if array needs to be allocated
            if (NULL == modelIter->texcoords) {
                if (3 <= modelIter->texCoordSize) {
                    //modelIter->texcoords = new Uv3f[modelIter->nrOfTexCoords];
                }
                else if (2 == modelIter->texCoordSize) {
                    modelIter->texcoords = new Uv2f[modelIter->nrOfTexCoords];
                }
                else {
                    modelIter->texcoords = new float[modelIter->nrOfTexCoords];
                }
                modelIter->nrOfTexCoords = 0;
            }
            //create stream object from string
            std::istringstream cstream(content);
            if (3 <= modelIter->texCoordSize) {
                /*Uv3f uvw;
                cstream >> uvw.u >> uvw.v >> uvw.w;
                ((Uv3f *)modelIter->texcoords)[modelIter->nrOfTexCoords++] = uvw;*/
            }
            else if (2 == modelIter->texCoordSize) {
                Uv2f uv;
                cstream >> uv.u >> uv.v;
                ((Uv2f *)modelIter->texcoords)[modelIter->nrOfTexCoords++] = uv;
            }
            else {
                float u;
                cstream >> u;
                ((float *)modelIter->texcoords)[modelIter->nrOfTexCoords++] = u;
            }
        }
        else if ("vn" == command) { //normal
            //check if array needs to be allocated
            if (NULL == modelIter->normals) {
                modelIter->normals = new Vector3f[modelIter->nrOfNormals];
                modelIter->nrOfNormals = 0;
            }
            //create stream object from string
            std::istringstream cstream(content);
            Vector3f n;
            //get normal data from stream and copy to normal array
            cstream >> n.x >> n.y >> n.z;
            modelIter->normals[modelIter->nrOfNormals++] = n;
        }
        else if ("p" == command) { //point
            meshIter = meshCommandRead(modelIter, meshIter, command, content, currentLine);
        }
        else if ("l" == command) { //line
            meshIter = meshCommandRead(modelIter, meshIter, command, content, currentLine);
        }
        else if ("f" == command) { //face
            meshIter = meshCommandRead(modelIter, meshIter, command, content, currentLine);
        }
        else if ("g" == command) { //group statement
            //ignore
        }
        else if ("o" == command) { //new model
            //if the current model has a different name that this, skip to next model, else do nothing
            if (modelIter->name != content) {
                modelIter++;
                meshIter = modelIter->meshes.begin();
            }
        }
        else if ("mtllib" == command) { //material library
            //load material library
        }
        else if ("usemtl" == command) { //use material with name
            //check if material with name exits
            //add to model
        }
        else {
            //unknown tag
        }
    } //while(!stream.eof())
    return true;
}

//----- model retrieval ------------------------------------------------------------------------------------------------

int ModelObj::getNrOfModels() const
{
    return models.size();
}

const ModelObj::Model & ModelObj::getModel(const unsigned int nr) const
{
    if (nr >= 0 && nr < models.size()) {
        return models.at(nr);
    }
    return Model();
}

//----- model management ------------------------------------------------------------------------------------------------

void ModelObj::freeModel(Model & model)
{
    if (NULL != model.vertices && 0 < model.nrOfVertices) {
        delete [] model.vertices;
        model.vertices = NULL;
        model.nrOfVertices = 0;
    }
    if (NULL != model.normals && 0 < model.nrOfNormals) {
        delete [] model.normals;
        model.normals = NULL;
        model.nrOfNormals = 0;
    }
    if (NULL != model.tangents && 0 < model.nrOfTangents) {
        delete [] model.tangents;
        model.tangents = NULL;
        model.nrOfTangents = 0;
    }
    if (NULL != model.texcoords && 0 < model.nrOfTexCoords) {
        delete [] model.texcoords;
        model.texcoords = NULL;
        model.nrOfTexCoords = 0;
        model.texCoordSize = 0;
    }
    //free all meshes in model
    std::vector<Mesh>::iterator iter = model.meshes.begin();
    if (model.meshes.end() != iter) {
        if (NULL != iter->vertexIndices) {
            delete [] iter->vertexIndices;
            iter->vertexIndices = NULL;
        }
        if (NULL != iter->normalIndices) {
            delete [] iter->normalIndices;
            iter->normalIndices = NULL;
        }
        if (NULL != iter->tangentIndices) {
            delete [] iter->tangentIndices;
            iter->tangentIndices = NULL;
        }
        if (NULL != iter->textureIndices) {
            delete [] iter->textureIndices;
            iter->textureIndices = NULL;
        }
        iter->nrOfIndices = 0;
        iter->nrOfPrimitives = 0;
        iter->primitiveType = (PrimitiveType)-1;
    }
}

void ModelObj::freeAll()
{
    //free all models
    std::vector<Model>::iterator iter = models.begin();
    if (models.end() != iter) {
        freeModel(*iter);
    }
    models.clear();
    materials.clear();
}

//----- destructor -----------------------------------------------------------------------------------------------------

ModelObj::~ModelObj(void)
{
    freeAll();
}
