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
    float time;
    aiVector3t<float> pos;
    aiQuaterniont<float> rot;
    aiVector3t<float> scale;
};

struct Joint
{
    std::string parentName;
    aiMatrix4x4 bindPose;
    std::vector<TimeStamp> stamps;
};

void FindParentName(std::map<std::string, Joint>& joints, aiNode* node)
{
    if (joints.find(node->mName.C_Str()) != joints.end())
    {
        // skip scaling->rotation->preRotation->translation
        std::string_view parentJointName = node->mParent->mParent->mParent->mParent->mParent->mName.C_Str();

        if (parentJointName != "RootNode")
            joints[node->mName.C_Str()].parentName = parentJointName;
    }

    for (UINT i = 0; i < node->mNumChildren; i++)
        FindParentName(joints, node->mChildren[i]);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

#ifdef CONSOLE_ENABLE
    RedirectIOToConsole();
#endif // CONSOLE_ENABLE

    Assimp::Importer importer;

    const aiScene* pScene = importer.ReadFile("..\\Content\\Meshes\\ani\\StickANi.fbx",
        aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_JoinIdenticalVertices);

    auto ani = pScene->mAnimations[0];
    auto mesh = pScene->mMeshes[0];
    
    std::vector<aiVertexWeight> weights[3];

    int idx = 0;

    weights[idx].resize((mesh->mBones[idx]->mNumWeights));
    memcpy(weights[idx].data(), mesh->mBones[idx]->mWeights, sizeof(aiVertexWeight) * weights[idx].size());
    idx++;

    weights[idx].resize((mesh->mBones[idx]->mNumWeights));
    memcpy(weights[idx].data(), mesh->mBones[idx]->mWeights, sizeof(aiVertexWeight) * weights[idx].size());
    idx++;

    weights[idx].resize((mesh->mBones[idx]->mNumWeights));
    memcpy(weights[idx].data(), mesh->mBones[idx]->mWeights, sizeof(aiVertexWeight) * weights[idx].size());

    std::vector<aiVector3t<float>> verts(mesh->mNumVertices);
    memcpy(verts.data(), mesh->mVertices, sizeof(aiVector3t<float>) * verts.size());

    std::vector< aiVector3t<float>> weightVerts(weights[2].size());
    for (size_t i = 0; i < weights[2].size(); i++)
        weightVerts[i] = verts[weights[1][i].mVertexId];

    std::vector<aiNodeAnim> aniChannels(ani->mNumChannels);
    for (size_t i = 0; i < aniChannels.size(); i++)
        aniChannels[i] = *ani->mChannels[i];
 
    std::map<std::string, Joint> joints;
    for (UINT i = 0; i < mesh->mNumBones; i++)
    {
        auto& traChannel = *ani->mChannels[i * 3];
        auto& rotChannel = *ani->mChannels[i * 3 + 1];
        auto& scaChannel = *ani->mChannels[i * 3 + 2];

        Joint& joint = joints[mesh->mBones[i]->mName.C_Str()];

        joint.bindPose = mesh->mBones[i]->mOffsetMatrix;

        joint.stamps.resize(traChannel.mNumPositionKeys);
        for (size_t i = 0; i < joint.stamps.size(); i++)
        {
            joint.stamps[i].time = (float)traChannel.mPositionKeys[i].mTime;

            joint.stamps[i].pos = traChannel.mPositionKeys[i].mValue;
            joint.stamps[i].rot = rotChannel.mRotationKeys[i].mValue;
            joint.stamps[i].scale = scaChannel.mScalingKeys[i].mValue;
        }
    }

    FindParentName(joints, pScene->mRootNode);

    //for (aiNode* node : nodes)
    //{
    //    if (joints.find(node->mName.C_Str()) == joints.end())
    //        continue;

    //    // skip scaling->rotation->preRotation->translation
    //    aiNode* parent = node->mParent->mParent->mParent->mParent->mParent;

    //    if (std::string_view(parent->mName.C_Str()) != "RootNode")
    //        joints[node->mName.C_Str()].parentName = parent->mName.C_Str();
    //}


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