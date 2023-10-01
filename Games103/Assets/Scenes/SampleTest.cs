using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SampleTest : MonoBehaviour
{
    public GameObject a;
    public GameObject b;
    public GameObject c;

    // Start is called before the first frame update
    void Start()
    {
        //Debug.Log($"A : {TransormToMat4(a.transform)}");
        //Debug.Log($"B : {TransormToMat4(b.transform)}");
        //Debug.Log($"C : {TransormToMat4(c.transform)}");
        //Debug.Log($"B*A : {TransormToMat4(b.transform) * TransormToMat4(a.transform)}");
        //Debug.Log($"A*B : {TransormToMat4(a.transform) * TransormToMat4(b.transform)}");
        //Debug.Log($"A*C : {TransormToMat4(a.transform) * TransormToMat4(c.transform)}");

        //Debug.Log($"a transform : {a.transform.TransformPoint(b.transform.position)}");
        //Debug.Log($"a transform : {a.transform.TransformPoint(b.transform.localPosition)}");

        //Debug.Log($"a transform : {TransormPoint(a.transform, b.transform.position)}");
        //Debug.Log($"a transform : {TransormPoint(a.transform, b.transform.localPosition)}");


        Matrix4x4 matrix4X4 = new Matrix4x4(new Vector4(3, 0, 0, 0),
                                            new Vector4(0, 4, 0, 3),
                                            new Vector4(3, 0, 3, 0),
                                            new Vector4(0, 2, 0, 0));

        Vector3 v = new Vector3(1, 2, 3);
        Vector4 v1 = new Vector4(1, 2, 3, 1);
        Vector4 v2 = new Vector4(1, 2, 3, 0);

        Debug.Log($"MultiplyPoint :{matrix4X4.MultiplyPoint(v).ToString()}");
        Debug.Log($"MultiplyPoint3x4 :{matrix4X4.MultiplyPoint3x4(v).ToString()}");
        Debug.Log($"MultiplyVector :{matrix4X4.MultiplyVector(v).ToString()}");
        Debug.Log($"Multiply :{matrix4X4 * v}");
        Debug.Log($"Multiply v1 :{matrix4X4 * v1}");
        Debug.Log($"Multiply v2 :{matrix4X4 * v2}");
    }

    // Update is called once per frame
    void Update()
    {

    }
    private Matrix4x4 TransormToMat4(Transform transform)
    {
        Vector3 x = transform.localRotation * Vector3.right;
        Vector3 y = transform.localRotation * Vector3.up;
        Vector3 z = transform.localRotation * Vector3.forward;

        x = x * transform.localScale.x;
        y = y * transform.localScale.y;
        z = z * transform.localScale.z;

        Vector3 p = transform.localPosition;
        Matrix4x4 res = new Matrix4x4();
        res[0, 0] = x.x;
        res[0, 1] = y.x;
        res[0, 2] = z.x;
        res[0, 3] = p.x;

        res[1, 0] = x.y;
        res[1, 1] = y.y;
        res[1, 2] = z.y;
        res[1, 3] = p.y;

        res[2, 0] = x.z;
        res[2, 1] = y.z;
        res[2, 2] = z.z;
        res[2, 3] = p.z;

        res[3, 0] = 0;
        res[3, 1] = 0;
        res[3, 2] = 0;
        res[3, 3] = 1;
        return res;
    }

    private Vector3 TransormPoint(Transform transform, Vector3 point)
    {
        Vector3 res = Vector3.zero;
        res = transform.localRotation * new Vector3(transform.localScale.x * point.x,
                                                    transform.localScale.y * point.y,
                                                    transform.localScale.z * point.z);
        res = res + transform.localPosition;
        return res;
    }
}
