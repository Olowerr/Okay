#define NOMINMAX
#define STB_IMAGE_IMPLEMENTATION

#include "Application.h"

#include <io.h>
#include <fcntl.h>
#include <iostream>

#define CONSOLE_ENABLE

#ifdef CONSOLE_ENABLE
void RedirectIOToConsole();
#endif

struct TimeStamp
{
    TimeStamp()
        :scale(1.f, 1.f, 1.f) { }

    float time = 0.f;
    aiVector3t<float> pos;
    aiQuaterniont<float> rot;
    aiVector3t<float> scale;
};

struct Joint
{
    std::string name;
    int parentIdx = -1;
    aiMatrix4x4 baseMatrix;
    std::vector<TimeStamp> stamps;
};

int FindJointIndex(std::vector<Joint>& joints, std::string_view name)
{
    for (int i = 0; i < (int)joints.size(); i++)
    {
        if (joints[i].name == name)
            return i;
    }

    return -1;
}

aiNode* GetParentNode(std::vector<Joint>& joints, aiNode* child)
{
    // Is root?
    if (!child->mParent)
        return nullptr;

    // Is the parent node a joint?
    if (FindJointIndex(joints, child->mParent->mName.C_Str()) != -1)
        return child->mParent;
    
    // Continue searching...
    return GetParentNode(joints, child->mParent);
}

void SetParents(std::vector<Joint>& joints, aiNode* node)
{
    int currentJointIdx = FindJointIndex(joints, node->mName.C_Str());
    if (currentJointIdx != -1)
    {
        aiNode* pParent = GetParentNode(joints, node);
        if (pParent)
        {
            joints[currentJointIdx].parentIdx = FindJointIndex(joints, pParent->mName.C_Str());
        }
    }

    for (UINT i = 0; i < node->mNumChildren; i++)
        SetParents(joints, node->mChildren[i]);
}

void FillNodes(std::unordered_map<std::string_view, aiNode*>& nodes, aiNode* parent)
{
    for (UINT i = 0; i < parent->mNumChildren; i++)
    {
        nodes[parent->mChildren[i]->mName.C_Str()] = parent->mChildren[i];
        FillNodes(nodes, parent->mChildren[i]);
    }
}

aiNodeAnim* FindAniNode(std::vector<aiNodeAnim*>& vec, std::string_view name, const std::string_view component)
{
    for (aiNodeAnim* node : vec)
    {
        std::string_view nodeName = node->mNodeName.C_Str();

        if (nodeName.find(name) != -1)
        {
            if (nodeName.find(component) != -1)
                return node;
        }
    }

    return nullptr;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

#ifdef CONSOLE_ENABLE
    RedirectIOToConsole();
#endif // CONSOLE_ENABLE

    Assimp::Importer importer;

    const aiScene* pScene = importer.ReadFile("..\\Content\\Meshes\\ani\\gobWalk.fbx",
        aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_JoinIdenticalVertices);
    
    auto ani = pScene->mAnimations[0];
    auto mesh = pScene->mMeshes[0];

    std::vector<aiNodeAnim*> aniNodes(ani->mNumChannels);
    memcpy(aniNodes.data(), ani->mChannels, sizeof(ani->mChannels) * ani->mNumChannels);

    ///////////////////

    std::unordered_map<std::string_view, aiNode*> nodes;
    FillNodes(nodes, pScene->mRootNode);

    std::vector<Joint> joints(mesh->mNumBones);
    
    for (UINT i = 0; i < mesh->mNumBones; i++)
    {
        joints[i].name = mesh->mBones[i]->mName.C_Str();
        joints[i].baseMatrix = mesh->mBones[i]->mOffsetMatrix;

        aiNodeAnim* traChannel = FindAniNode(aniNodes, joints[i].name.c_str(), "Translation");
        aiNodeAnim* rotChannel = FindAniNode(aniNodes, joints[i].name.c_str(), "Rotation");
        aiNodeAnim* scaChannel = FindAniNode(aniNodes, joints[i].name.c_str(), "Scaling");

        const size_t numKeys = traChannel ? traChannel->mNumPositionKeys : rotChannel ? rotChannel->mNumRotationKeys : scaChannel ? scaChannel->mNumScalingKeys : 0;
        joints[i].stamps.resize(numKeys);

        for (size_t k = 0; k < numKeys; k++)
        {
            if      (traChannel) joints[i].stamps[k].time = (float)traChannel->mPositionKeys[k].mTime;
            else if (rotChannel) joints[i].stamps[k].time = (float)rotChannel->mRotationKeys[k].mTime;
            else if (scaChannel) joints[i].stamps[k].time = (float)scaChannel->mScalingKeys[k].mTime;
                   
            if (traChannel) joints[i].stamps[k].pos   = traChannel->mPositionKeys[k].mValue;
            if (rotChannel) joints[i].stamps[k].rot   = rotChannel->mRotationKeys[k].mValue;
            if (scaChannel) joints[i].stamps[k].scale = scaChannel->mScalingKeys[k].mValue;
        }
    }

    SetParents(joints, pScene->mRootNode);

    struct VtxWeight
    {
        UINT jointIdx[4]{};
        float weight[4]{};
    };

    // size is temp
    std::vector<VtxWeight> weights(mesh->mNumVertices);
    for (UINT i = 0; i < mesh->mNumBones; i++)
    {
        for (UINT k = 0; k < mesh->mBones[i]->mNumWeights; k++)
        {
            aiVertexWeight& aiWeight = mesh->mBones[i]->mWeights[k];
            VtxWeight& currVertex = weights[aiWeight.mVertexId];

            if      (currVertex.weight[0] == 0.f) { currVertex.weight[0] = aiWeight.mWeight; currVertex.jointIdx[0] = i; }
            else if (currVertex.weight[1] == 0.f) { currVertex.weight[1] = aiWeight.mWeight; currVertex.jointIdx[1] = i; }
            else if (currVertex.weight[2] == 0.f) { currVertex.weight[2] = aiWeight.mWeight; currVertex.jointIdx[2] = i; }
            else if (currVertex.weight[3] == 0.f) { currVertex.weight[3] = aiWeight.mWeight; currVertex.jointIdx[3] = i; }

        }
    }

    for (size_t i = 0; i < joints.size(); i++)
    {
        if (joints[i].parentIdx >= (int)i)
        {
            printf("Wtf: %d\n", (int)i);
            
        }
    }
    
    return 0;

    Application application;

	if (!application.Initiate())
		return -1;
	
    application.Run();
    application.Shutdown();
	 
	return 0;
}


#ifdef CONSOLE_ENABLE
void RedirectIOToConsole()
{

    //Create a console for this application
    AllocConsole();

    // Get STDOUT handle
    HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    int SystemOutput = _open_osfhandle(intptr_t(ConsoleOutput), _O_TEXT);
    FILE* COutputHandle = _fdopen(SystemOutput, "w");

    // Get STDERR handle
    HANDLE ConsoleError = GetStdHandle(STD_ERROR_HANDLE);
    int SystemError = _open_osfhandle(intptr_t(ConsoleError), _O_TEXT);
    FILE* CErrorHandle = _fdopen(SystemError, "w");

    //make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
    std::ios::sync_with_stdio(true);

    // Redirect the CRT standard input, output, and error handles to the console
    freopen_s(&COutputHandle, "CONOUT$", "w", stdout);
    freopen_s(&CErrorHandle, "CONOUT$", "w", stderr);

    //Clear the error state for each of the C++ standard stream objects. We need to do this, as
    //attempts to access the standard streams before they refer to a valid target will cause the
    //iostream objects to enter an error state. In versions of Visual Studio after 2005, this seems
    //to always occur during startup regardless of whether anything has been read from or written to
    //the console or not.
    std::wcout.clear();
    std::cout.clear();
    std::wcerr.clear();
    std::cerr.clear();

}
#endif