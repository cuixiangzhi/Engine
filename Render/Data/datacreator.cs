using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.IO;

public class datacreator : MonoBehaviour
{
    public MeshFilter mPlayerMesh;
    public MeshFilter mGroundMesh;
    public Texture2D mPlayerTexture;
    public Texture2D mGroundTexture;
    public List<Texture2D> mSkyboxTexture;

    void Start()
    {
        if (mPlayerMesh != null) WriteMeshToFile(mPlayerMesh, "playermesh");
        if (mGroundMesh != null) WriteMeshToFile(mGroundMesh, "groundmesh");
        if (mPlayerTexture != null) WriteTextureToFile(mPlayerTexture, "playertexture");
        if (mGroundTexture != null) WriteTextureToFile(mGroundTexture, "groundtexture");
        if (mSkyboxTexture != null) WriteCubemapToFile(mSkyboxTexture, "skyboxtexture");
    }

    void WriteCubemapToFile(List<Texture2D> cubemap, string fileName)
    {
        string filePath = "E:/Renderer/Data/" + fileName + ".obj";
        if(cubemap != null && cubemap.Count == 6)
        {
            FileStream writer = new FileStream(filePath, FileMode.OpenOrCreate, FileAccess.Write);
            writer.SetLength(0);
            byte[] buffer = new byte[4];
            writer.Write(FillBuffer(buffer, cubemap[0].width), 0, 4);
            writer.Write(FillBuffer(buffer, cubemap[0].height), 0, 4);
            for (CubemapFace face = CubemapFace.PositiveX; face <= CubemapFace.NegativeZ; ++face)
            {
                for (int i = 0; i < cubemap[0].height; ++i)
                {
                    for (int j = 0; j < cubemap[0].width; j++)
                    {
                        var curFace = cubemap[(int)face];
                        Color color = curFace.GetPixel(j, i);
                        writer.Write(FillBuffer(buffer, color), 0, 4);
                    }
                }
            }
            writer.Close();
        }
    }

    void WriteTextureToFile(Texture2D texture, string fileName)
    {
        string filePath = "E:/Renderer/Data/" + fileName + ".obj";
        if(texture != null)
        {
            StreamWriter writer = new StreamWriter(filePath, false);
            writer.WriteLine("{0} {1}", texture.width, texture.height);
            for (int i = 0; i < texture.height; ++i)
            {
                for (int j = 0; j < texture.width; j++)
                {
                    Color color = texture.GetPixel(j, i);
                    writer.WriteLine("p {0} {1} {2} {3}", color.r, color.g, color.b, color.a);
                }
            }
            writer.Close();
        }
    }

    void WriteMeshToFile(MeshFilter mf, string fileName)
    {
        string filePath = "E:/Renderer/Data/" + fileName + ".obj";
        if (mf != null && mf.sharedMesh != null)
        {
            var mesh = mf.sharedMesh;
            StreamWriter writer = new StreamWriter(filePath, false);
            writer.WriteLine("{0} {1}", mesh.vertexCount, mesh.triangles.Length / 3);
            for (int idx = 0; idx < mesh.vertexCount; idx++)
            {
                Vector3 position = mesh.vertices[idx];
                Vector3 normal = mesh.normals[idx];
                Color color = position.x < 0 ? Color.red : Color.green;
                Vector2 uv = mesh.uv[idx];

                writer.WriteLine("v {0} {1} {2} {3} {4} {5} {6} {7} {8} {9} {10}", position.x, position.y, position.z, normal.x, normal.y, normal.z, color.r, color.g, color.b, uv.x, uv.y);
            }
            for (int idx = 0; idx < mesh.triangles.Length; idx += 3)
            {
                writer.WriteLine("f {0} {1} {2}", mesh.triangles[idx], mesh.triangles[idx + 1], mesh.triangles[idx + 2]);
            }

            writer.Close();
        }
    }

    byte[] FillBuffer(byte[] buffer,int value)
    {
        buffer[0] = (byte)(value >> 24 & 0xFF);
        buffer[1] = (byte)(value >> 16 & 0xFF);
        buffer[2] = (byte)(value >> 8 & 0xFF);
        buffer[3] = (byte)(value >> 0 & 0xFF);
        return buffer;
    }

    byte[] FillBuffer(byte[] buffer, Color32 color)
    {
        buffer[0] = color.r;
        buffer[1] = color.g;
        buffer[2] = color.b;
        buffer[3] = color.a;
        return buffer;
    }
}
